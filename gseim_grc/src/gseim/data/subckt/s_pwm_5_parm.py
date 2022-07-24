def s_pwm_5_parm(dict1):
    fc = float(dict1['fc'])

    t_offset = 0.5/fc

    dict1['t_offset'] = '%11.4e' % (t_offset)
    print('s_pwm_5_parm: parameters computed')
