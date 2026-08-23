# Sensor Fault Detection

## PMS7003T Fault-Injection Test

To evaluate the sensor fault-detection mechanism, a controlled fault was introduced in the PMS7003T particulate-matter sensor by blocking its inlet using a piece of tape.

### Fault Detection Sequence

The video demonstrates the complete transition from normal operation to a detected sensor fault and back to the healthy state.

| Time | Observation |
|---|---|
| **00:35** | After the PMS7003T inlet is blocked with tape, the sensor output reaches **zero**, indicating abnormal sensor behavior. |
| **00:35 onward** | The **fault score starts increasing** as the system detects a deviation between the sensor-derived AQI and the secondary-sensing prediction. |
| **01:30** | The fault score reaches the configured fault threshold and the system reports the PMS7003T sensor as **FAULTY**. |
| **02:10** | The tape is removed from the sensor inlet, allowing the PMS7003T to operate normally again. |
| **02:22** | As the sensor resumes normal operation, the **fault score starts decreasing**. |
| **03:38** | The fault score falls below the configured threshold and the sensor status changes back to **HEALTHY**. |

### Observed Behavior

The experiment demonstrates that the system can:

1. Detect abnormal PMS7003T behavior after a deliberate inlet blockage.
2. Increase the fault score while the sensor remains in the faulty condition.
3. Mark the sensor as **FAULTY** after the fault score crosses the threshold.
4. Recover from the fault condition after the blockage is removed.
5. Reduce the fault score during normal sensor operation.
6. Restore the sensor status to **HEALTHY** once the fault score falls below the threshold.

### Demonstration Video

The complete fault-injection and recovery sequence is demonstrated in the accompanying video.

**Experiment sequence:**

`Normal operation → Inlet blocked → Zero PM reading → Fault score increases → FAULTY → Tape removed → Fault score decreases → HEALTHY`
