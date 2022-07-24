def bridgeless_boost_pfc_parm(dict1):
    import numpy as np

    F = float(dict1['F'])
    C = float(dict1['C'])
    L = float(dict1['L'])
    Rl = float(dict1['Rl'])
    fs = float(dict1['fs'])
    Ri_F = float(dict1['Ri_F'])
    Rv_F = float(dict1['Rv_F'])

    Vin = 120.0*np.sqrt(2)
    Ri_Ts = 1/Ri_F
    Ri_Tp = Ri_Ts/2.0
    Ri_Ti = (2/Rl)*Ri_Tp
    Ri_Tn = L/Rl

    Ri_Kp = Ri_Tn/Ri_Ti
    Ri_Ki = 1/Ri_Ti

    Rv_Ts = 1/Rv_F
    Rv_Tp = (Rv_Ts/2.0)*10.0

    Rv_Ti = (8/C)*Rv_Tp*Rv_Tp
    Rv_Tn = 4*Rv_Tp

    Rv_Kp = Rv_Tn/Rv_Ti
    Rv_Ki = 1/Rv_Ti

    b0_1 = (2.0*np.pi*F)**2
    t_offset_1 = (1/F)/2

    dict1['Vin'] = '%11.4e' % (Vin)

    dict1['Ri_Ts'] = '%11.4e' % (Ri_Ts)
    dict1['Ri_Tp'] = '%11.4e' % (Ri_Tp)
    dict1['Ri_Ti'] = '%11.4e' % (Ri_Ti)
    dict1['Ri_Tn'] = '%11.4e' % (Ri_Tn)
    dict1['Ri_Kp'] = '%11.4e' % (Ri_Kp)
    dict1['Ri_Ki'] = '%11.4e' % (Ri_Ki)

    dict1['Rv_Ts'] = '%11.4e' % (Rv_Ts)
    dict1['Rv_Tp'] = '%11.4e' % (Rv_Tp)
    dict1['Rv_Ti'] = '%11.4e' % (Rv_Ti)
    dict1['Rv_Tn'] = '%11.4e' % (Rv_Tn)
    dict1['Rv_Kp'] = '%11.4e' % (Rv_Kp)
    dict1['Rv_Ki'] = '%11.4e' % (Rv_Ki)

    dict1['b0_1'] = '%11.4e' % (b0_1)
    dict1['t_offset_1'] = '%11.4e' % (t_offset_1)

    print('bridgeless_boost_pfc_parm: parameters computed')
