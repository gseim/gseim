def three_level_boost_pfc_parm(dict1):
    import numpy as np
    Vin = float(dict1['Vin'])
    fac = float(dict1['fac'])
    Cout = float(dict1['Cout'])
    Rl = float(dict1['Rl'])
    L = float(dict1['L'])

    Vsp = Vin*np.sqrt(2.0)
    x = 4.0*np.pi*fac
    a0_filter = x*x
    b0_filter = a0_filter
    b1_filter = x/10.0

    V_F = 500.0
    V_Ts = 1.0/V_F
    V_Tp = V_Ts

    V_T2 = Cout
    V_Ti = 8.0/V_T2*V_Tp*V_Tp
    V_Tn = 4.0*V_Tp
    V_Kp = V_Tn/V_Ti
    V_Ki = 1.0/V_Ti

    C_F = 100e3
    C_Ts = 1.0/C_F
    C_Tp = C_Ts

    C_K1 = 1.0/Rl
    C_Tn = L/Rl
    C_Ti = 2.0*C_K1*C_Tp
    C_Kp = C_Tn/C_Ti
    C_Ki = 1.0/C_Ti

    dict1['Vsp'] = '%11.4e' % (Vsp)
    dict1['a0_filter'] = '%11.4e' % (a0_filter)
    dict1['b0_filter'] = '%11.4e' % (b0_filter)
    dict1['b1_filter'] = '%11.4e' % (b1_filter)
    dict1['V_Kp'] = '%11.4e' % (V_Kp)
    dict1['V_Ki'] = '%11.4e' % (V_Ki)
    dict1['C_Kp'] = '%11.4e' % (C_Kp)
    dict1['C_Ki'] = '%11.4e' % (C_Ki)

    print('three_level_boost_pfc_parm: parameters computed')
