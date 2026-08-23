# Future Work

## Kitchen-Based Secondary Sensing and Activity Prediction

The next phase of PolluSenseCheck is focused on extending the secondary-sensing approach to indoor kitchen environments.

Kitchen activities such as boiling, frying, cooking, reheating, and baking can produce characteristic changes in environmental and pollutant measurements. The objective is to use this contextual information to improve both **kitchen activity prediction** and **sensor fault detection**.

### Planned Direction

```text
Kitchen Sensor Data
        |
        v
Kitchen Activity Prediction
        |
        v
Activity-Dependent Environmental Pattern
        |
        v
Secondary Sensing Model
        |
        +----------------------+
        |                      |
        v                      v
Actual Sensor Reading    Predicted Reading
        |                      |
        +----------+-----------+
                   |
                   v
            Sensor Comparison
                   |
                   v
             Fault Detection
