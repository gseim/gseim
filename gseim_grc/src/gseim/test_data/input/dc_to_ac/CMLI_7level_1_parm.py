def CMLI_7level_1_parm(dict1):
    f_sw = float(dict1['f_sw'])
    n_levels = 3

    T = 1/f_sw
    n2 = 2*n_levels
    delta = T/n2

    t_offset_1 = -delta
    t_offset_2 = -2*delta

    T_avg = T

    print('CMLI_7level_1_parm.py: t_offset_1:', t_offset_1)
    print('CMLI_7level_1_parm.py: t_offset_2:', t_offset_2)
    print('CMLI_7level_1_parm.py: T_avg:', T_avg)

    dict1['t_offset_1'] = '%11.4e' % (t_offset_1)
    dict1['t_offset_2'] = '%11.4e' % (t_offset_2)
    dict1['T_avg'] = '%11.4e' % (T_avg)
    print('CMLI_7level_1_parm.py: parameters computed')
