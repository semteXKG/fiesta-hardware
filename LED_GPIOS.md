# LED GPIO Reference

LED control was removed from the codebase. This file documents the pin assignments for future re-implementation.

## LED Bar (10 segments)

| Segment | GPIO |
|---------|------|
| 0       | 15   |
| 1       | 2    |
| 2       | 0    |
| 3       | 4    |
| 4       | 16   |
| 5       | 17   |
| 6       | 5    |
| 7       | 18   |
| 8       | 19   |
| 9       | 21   |

## PWM Brightness

| Function        | GPIO |
|-----------------|------|
| PWM brightness  | 22   |

The brightness was controlled via MCPWM with a 1 MHz timebase and a 100-tick period.
