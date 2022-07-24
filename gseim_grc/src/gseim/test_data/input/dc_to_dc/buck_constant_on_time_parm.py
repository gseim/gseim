def buck_constant_on_time_parm(dict1):
    Vin = float(dict1['Vin'])
    V = float(dict1['V'])
    L = float(dict1['L'])
    Co = float(dict1['Co'])
    RL = float(dict1['RL'])
    fsw = float(dict1['fsw'])

    Vref = V

    Rv_F = fsw/5
    Rv_Ts = 1/Rv_F
    Rv_Tp = (Rv_Ts/2)*10

    Rv_Ti = (8/Co)*Rv_Tp*Rv_Tp
    Rv_Tn = 4*Rv_Tp

    kp = Rv_Tn/Rv_Ti
    ki = 1/Rv_Ti

    ton = V/(Vin*fsw)
    toff_min = ton
    t3 = ton + toff_min + 0.01*ton

    dict1['Vref'] = '%11.4e' % (Vref)
    dict1['Rv_F'] = '%11.4e' % (Rv_F)
    dict1['Rv_Ts'] = '%11.4e' % (Rv_Ts)
    dict1['Rv_Tp'] = '%11.4e' % (Rv_Tp)
    dict1['Rv_Ti'] = '%11.4e' % (Rv_Ti)
    dict1['Rv_Tn'] = '%11.4e' % (Rv_Tn)
    dict1['kp'] = '%11.4e' % (kp)
    dict1['ki'] = '%11.4e' % (ki)
    dict1['ton'] = '%11.4e' % (ton)
    dict1['toff_min'] = '%11.4e' % (toff_min)
    dict1['t3'] = '%11.4e' % (t3)

    print('Vin:', Vin)
    print('Vref:', Vref)
    print('Rv_F:', Rv_F)
    print('Rv_Ts:', Rv_Ts)
    print('Rv_Tp:', Rv_Tp)
    print('Rv_Ti:', Rv_Ti)
    print('Rv_Tn:', Rv_Tn)
    print('kp:', kp)
    print('ki:', ki)
    print('ton:', ton)
    print('toff_min:', toff_min)

    print('buck_constant_on_time_parm: parameters computed')
