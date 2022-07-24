def s_gate_pulse_2_parm(dict1):
    print('s_gate_pulse_2_parm: starting...')
    beta      = float(dict1['beta'     ])
    frequency = float(dict1['frequency'])

    print('s_gate_pulse_2_parm: beta:', beta)
    print('s_gate_pulse_2_parm: frequency:', frequency)

    t_period = 1.0/frequency
    T = (beta/360.0)*t_period

    print('s_gate_pulse_2_parm: T:', T)

    dict1['T'] = '%11.4e' % (T)
    print('s_gate_pulse_2_parm: parameters computed')
