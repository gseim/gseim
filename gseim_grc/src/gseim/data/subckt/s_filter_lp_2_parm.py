def s_filter_lp_2_parm(dict1):
    from numpy import pi

    print('s_filter_lp_2_parm starts...')

    fc = float(dict1['fc'])
    k  = float(dict1['k'])
    xi = float(dict1['xi'])

    omg_c = 2*pi*fc;
    a0 = k*omg_c*omg_c;
    b0 = omg_c*omg_c;
    b1 = 2.0*xi*omg_c;
    b2 = 1.0;

    print('s_filter_lp_2_parm: a0:', a0)
    print('s_filter_lp_2_parm: b0:', b0)
    print('s_filter_lp_2_parm: b1:', b1)
    print('s_filter_lp_2_parm: b2:', b2)

    dict1['a0'] = '%11.4e' % (a0)
    dict1['b0'] = '%11.4e' % (b0)
    dict1['b1'] = '%11.4e' % (b1)
    dict1['b2'] = '%11.4e' % (b2)
    print('s_filter_lp_2_parm: parameters computed')
