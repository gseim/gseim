def s_phase_shift_pwm_parm(dict1):
    fc = float(dict1['fc'])

    t0_1 = 0.5/fc
    T_mono = 0.02/fc

    dict1['t0_1'] = '%11.4e' % (t0_1)
    dict1['T_mono'] = '%11.4e' % (T_mono)
    print('t0_1:', t0_1)
    print('T_mono:', T_mono)
    print('s_phase_shift_pwm: parameters computed')
