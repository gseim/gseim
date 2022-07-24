def s_clock_2a_parm(dict1):
    D      = float(dict1['D'])
    f_hz   = float(dict1['f_hz'])
    offset = float(dict1['offset'])
    x_high = float(dict1['x_high'])

    T = 1.0/f_hz

    x0 = (1.0-D)*x_high;
    t0 = offset + 0.5*D*T;

    print('s_clock_2a_parm: x0:', x0)
    print('s_clock_2a_parm: t0:', t0)

    dict1['x0'] = '%11.4e' % (x0)
    dict1['t0'] = '%11.4e' % (t0)
    print('s_clock_2a_parm: parameters computed')
