Control Protocols
=================

Due to the different control type, we divide control protocols into 4 parts: Motor, Holder, Sensor, Function. The format of control message starts with `[` and ends with `]`, the letter `[` and `]` are **identifier** of control message. 

The example of control message is like this:
> [command_string]

1. Motor Related
----------------

The prefix of motor related control message is `m`

- **Forward** command is `f`
- **Backward** command is `b`
- **Turn Left** command is `l`
- **Turn Right** command is `r`
- **Speed Up** command is `u`
- **Speed Down** command is `d`
- **Stop** command is `e`

2. Holder Related
-----------------

The prefix of holder related control message is `h`

- **Turn Up** command is `u`
- **Turn Down** command is `d`
- **Turn Left** command is `l`
- **Turn Right** command is `r`
- **Stop** command is `e`

3. Sensor Related
-----------------

The prefix of sensor related control message is `s`

4. Function Related
-------------------

The prefix of function related control message is `f`

