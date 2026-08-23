/******************************************************************************
 * File Name   : tinyml.cpp
 * Project     : PolluSense
 * Description :
 * TinyML Classification Engine
 *
 * This module:
 *  - Loads PM2.5 and CO TensorFlow Lite models
 *  - Creates separate interpreters
 *  - Performs float32 inference
 *  - Returns AQI class using ArgMax
 *
 * NOTE:
 * Input features are already Min-Max scaled by ml_preprocessing.
 * No feature scaling is performed inside this module.
 ******************************************************************************/

#include "tinyml.h"

#include "pm_model.h"
#include "co_model.h"

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_log.h"

#include <cstring>

using namespace tflite;

/******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static esp_err_t create_interpreters(void);

static void register_ops(void);

/******************************************************************************
 * Runtime Objects
 ******************************************************************************/

namespace
{

/*------------------------------------------------------------------
 * Embedded Models
 *-----------------------------------------------------------------*/

const Model *pm_model_ptr = nullptr;

const Model *co_model_ptr = nullptr;

/*------------------------------------------------------------------
 * Tensor Arenas
 *-----------------------------------------------------------------*/

alignas(16)
uint8_t pm_tensor_arena[PM_TENSOR_ARENA_SIZE];

alignas(16)
uint8_t co_tensor_arena[CO_TENSOR_ARENA_SIZE];

/*------------------------------------------------------------------
 * Operator Resolvers
 *-----------------------------------------------------------------*/

MicroMutableOpResolver<4> pm_resolver;

MicroMutableOpResolver<4> co_resolver;

/*------------------------------------------------------------------
 * Interpreters
 *-----------------------------------------------------------------*/

MicroInterpreter *pm_interpreter = nullptr;

MicroInterpreter *co_interpreter = nullptr;

/*------------------------------------------------------------------
 * Input Tensors
 *-----------------------------------------------------------------*/

TfLiteTensor *pm_input = nullptr;

TfLiteTensor *co_input = nullptr;

/*------------------------------------------------------------------
 * Output Tensors
 *-----------------------------------------------------------------*/

TfLiteTensor *pm_output = nullptr;

TfLiteTensor *co_output = nullptr;

/*------------------------------------------------------------------
 * Runtime Context
 *-----------------------------------------------------------------*/

tinyml_context_t ctx =
{
    .initialized = false,
    .model = MODEL_PM
};

}

/******************************************************************************
 * Register TensorFlow Lite Operators
 ******************************************************************************/

static void register_ops(void)
{
    /* PM Model */

    pm_resolver.AddFullyConnected();
    pm_resolver.AddRelu();
    pm_resolver.AddReshape();
    pm_resolver.AddSoftmax();

    /* CO Model */

    co_resolver.AddFullyConnected();
    co_resolver.AddRelu();
    co_resolver.AddReshape();
    co_resolver.AddSoftmax();
}

/******************************************************************************
 * Create Interpreters
 ******************************************************************************/

static esp_err_t create_interpreters(void)
{
    /******************************************************************
     * PM MODEL
     ******************************************************************/

    static MicroInterpreter pm_interp(

        pm_model_ptr,

        pm_resolver,

        pm_tensor_arena,

        PM_TENSOR_ARENA_SIZE,

        nullptr,

        nullptr,

        false

    );

    pm_interpreter = &pm_interp;

    if(pm_interpreter->AllocateTensors() != kTfLiteOk)
    {
        MicroPrintf("PM AllocateTensors Failed");

        return ESP_FAIL;
    }

    pm_input = pm_interpreter->input(0);

    pm_output = pm_interpreter->output(0);

    if(pm_input == nullptr || pm_output == nullptr)
    {
        MicroPrintf("PM Tensor Error");

        return ESP_FAIL;
    }

    /******************************************************************
     * CO MODEL
     ******************************************************************/

    static MicroInterpreter co_interp(

        co_model_ptr,

        co_resolver,

        co_tensor_arena,

        CO_TENSOR_ARENA_SIZE,

        nullptr,

        nullptr,

        false

    );

    co_interpreter = &co_interp;

    if(co_interpreter->AllocateTensors() != kTfLiteOk)
    {
        MicroPrintf("CO AllocateTensors Failed");

        return ESP_FAIL;
    }

    co_input = co_interpreter->input(0);

    co_output = co_interpreter->output(0);

    if(co_input == nullptr || co_output == nullptr)
    {
        MicroPrintf("CO Tensor Error");

        return ESP_FAIL;
    }

    return ESP_OK;
}
/******************************************************************************
 * Initialize TinyML Engine
 ******************************************************************************/

esp_err_t tinyml_init(void)
{
    /******************************************************************
     * Already Initialized
     ******************************************************************/

    if(ctx.initialized)
    {
        return ESP_OK;
    }

    /******************************************************************
     * Register Operators
     ******************************************************************/

    register_ops();

    /******************************************************************
     * Load PM Model
     ******************************************************************/

    pm_model_ptr = GetModel(pm_model_float32_tflite);

    if(pm_model_ptr == nullptr)
    {
        MicroPrintf("Failed to Load PM Model");

        return ESP_FAIL;
    }

    if(pm_model_ptr->version() != TFLITE_SCHEMA_VERSION)
    {
        MicroPrintf("PM Model Schema Mismatch");

        return ESP_FAIL;
    }

    /******************************************************************
     * Load CO Model
     ******************************************************************/

    co_model_ptr = GetModel(co_model_float32_tflite);

    if(co_model_ptr == nullptr)
    {
        MicroPrintf("Failed to Load CO Model");

        return ESP_FAIL;
    }

    if(co_model_ptr->version() != TFLITE_SCHEMA_VERSION)
    {
        MicroPrintf("CO Model Schema Mismatch");

        return ESP_FAIL;
    }

    /******************************************************************
     * Create Interpreters
     ******************************************************************/

    if(create_interpreters() != ESP_OK)
    {
        MicroPrintf("Interpreter Creation Failed");

        return ESP_FAIL;
    }

    /******************************************************************
     * Default Active Model
     ******************************************************************/

    ctx.model = MODEL_PM;

    ctx.initialized = true;

    MicroPrintf("TinyML Classification Engine Initialized");

    return ESP_OK;
}

/******************************************************************************
 * Select Active Model
 ******************************************************************************/

esp_err_t tinyml_load_model(
    tinyml_model_t model)
{
    if(!ctx.initialized)
    {
        MicroPrintf("TinyML Not Initialized");

        return ESP_FAIL;
    }

    switch(model)
    {
        case MODEL_PM:

            ctx.model = MODEL_PM;

            break;

        case MODEL_CO:

            ctx.model = MODEL_CO;

            break;

        default:

            MicroPrintf("Invalid Model");

            return ESP_FAIL;
    }

    return ESP_OK;
}
/******************************************************************************
 * Run TinyML Classification
 ******************************************************************************/

esp_err_t tinyml_predict(
    const float input[FEATURE_SIZE],
    uint8_t *predicted_class)
{
    /******************************************************************
     * Check Initialization
     ******************************************************************/

    if(!ctx.initialized)
    {
        MicroPrintf("TinyML Not Initialized");

        return ESP_FAIL;
    }

    if(predicted_class == nullptr)
    {
        MicroPrintf("Prediction Pointer NULL");

        return ESP_FAIL;
    }

    /******************************************************************
     * Select Active Interpreter
     ******************************************************************/

    MicroInterpreter *interpreter = nullptr;

    TfLiteTensor *input_tensor = nullptr;

    TfLiteTensor *output_tensor = nullptr;

    uint8_t num_classes = 0;

    if(ctx.model == MODEL_PM)
    {
        interpreter = pm_interpreter;

        input_tensor = pm_input;

        output_tensor = pm_output;

        num_classes = 5;
    }
    else
    {
        interpreter = co_interpreter;

        input_tensor = co_input;

        output_tensor = co_output;

        num_classes = 3;
    }

    if(interpreter == nullptr ||
       input_tensor == nullptr ||
       output_tensor == nullptr)
    {
        MicroPrintf("Interpreter Error");

        return ESP_FAIL;
    }

    /******************************************************************
     * Copy Features
     *
     * NOTE:
     * Features are ALREADY Min-Max scaled by ml_preprocess().
     * DO NOT scale them again.
     ******************************************************************/

    for(uint32_t i = 0; i < FEATURE_SIZE; i++)
    {
        input_tensor->data.f[i] = input[i];
    }

    /******************************************************************
     * Run Inference
     ******************************************************************/

    if(interpreter->Invoke() != kTfLiteOk)
    {
        MicroPrintf("Inference Failed");

        return ESP_FAIL;
    }

    /******************************************************************
     * ArgMax Classification
     ******************************************************************/

    uint8_t best_class = 0;

    float best_probability = output_tensor->data.f[0];

    for(uint8_t i = 1; i < num_classes; i++)
    {
        if(output_tensor->data.f[i] > best_probability)
        {
            best_probability = output_tensor->data.f[i];

            best_class = i;
        }
    }

    /******************************************************************
     * Debug Output
     ******************************************************************/

    MicroPrintf("Classification Output");

    for(uint8_t i = 0; i < num_classes; i++)
    {
        MicroPrintf("Class %d : %.9f",
            i,
            output_tensor->data.f[i]);
    }

    MicroPrintf("Predicted Class : %d",
                best_class);

    *predicted_class = best_class;

    return ESP_OK;
}

/******************************************************************************
 * Deinitialize TinyML
 ******************************************************************************/

void tinyml_deinit(void)
{
    ctx.initialized = false;

    ctx.model = MODEL_PM;

    pm_model_ptr = nullptr;
    co_model_ptr = nullptr;

    pm_interpreter = nullptr;
    co_interpreter = nullptr;

    pm_input = nullptr;
    pm_output = nullptr;

    co_input = nullptr;
    co_output = nullptr;

    MicroPrintf("TinyML Deinitialized");
}