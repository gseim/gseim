def CMLI_5level_1_parm(dict1):
    f_sw = float(dict1['f_sw'])
    n_levels = 2

    T = 1/f_sw
    n2 = 2*n_levels
    delta = T/n2

    t_offset_1 = -delta

    print('CMLI_5level_1_parm.py: t_offset_1:', t_offset_1)

    dict1['t_offset_1'] = '%11.4e' % (t_offset_1)
    print('CMLI_5level_1_parm.py: parameters computed')
