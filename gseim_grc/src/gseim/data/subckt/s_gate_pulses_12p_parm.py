def s_gate_pulses_12p_parm(dict1):
    print('s_gate_pulses_12p_parm: starting...')
    beta      = float(dict1['beta'     ])
    frequency = float(dict1['frequency'])

    print('s_gate_pulse_2s_12parm: beta:', beta)
    print('s_gate_pulse_2s_12parm: frequency:', frequency)

    t_period = 1.0/frequency
    T = (beta/360.0)*t_period

    print('s_gate_pulses_12p_parm: T:', T)

    dict1['T'] = '%11.4e' % (T)
    print('s_gate_pulses_12p_parm: parameters computed')
