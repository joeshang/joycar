Control Protocols
=================

The format of control message starts with `[` and ends with `]`, the letter `[` and `]` are **identifier** of control message. 

The example of control message is like this:
> [command_string]

Due to the different control type, we divide control protocols into 4 **Classes**: *Motor*, *Holder*, *Sensor*, *Function*. Every class has various **Types** of command, and every type has its own **Extra** data. So the structure of `command_string` is as below:

- **Command Class** at position 0 with size 1 byte.
- **Command Type** at position 1 with size 1 byte.
- **Extra Data** start at position 2 and end at `]`.

1. Motor Related
----------------

The prefix of motor related control message is `m`.

- **Forward** command is `f`.
- **Backward** command is `b`.
- **Turn Left** command is `l`.
- **Turn Right** command is `r`.
- **Set Speed** command is `s` with speed string.
- **Stop** command is `t`.

2. Holder Related
-----------------

The prefix of holder related control message is `h`.

- **Turn Up** command is `u`.
- **Turn Down** command is `d`.
- **Turn Left** command is `l`.
- **Turn Right** command is `r`.
- **Set Speed** command is `s` with speed string.
- **Stop** command is `t`.

3. Sensor Related
-----------------

The prefix of sensor related control message is `s`.

4. Function Related
-------------------

The prefix of function related control message is `f`.

