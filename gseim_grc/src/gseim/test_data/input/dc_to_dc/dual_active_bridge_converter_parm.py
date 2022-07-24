def dual_active_bridge_converter_parm(dict1):
    fc = float(dict1['fc'])
    Vout = float(dict1['Vout'])
    Pinitial = float(dict1['Pinitial'])

    Rload = Vout*Vout/Pinitial

    Ts = 50/fc
    dt = 0.001*Ts

    Kp = 0.11173*0.015
    Ki = 0.11173
    Kd = 0
    N = 0

    b0 = Kp*(1+N*Ts) + Ki*Ts*(1+N*Ts) + Kd*N
    b1 = -(Kp*(2+N*Ts) + Ki*Ts + 2*Kd*N)
    b2 = Kp + Kd*N
    a0 = 1+N*Ts
    a1 = -(2+N*Ts)
    a2 = 1

    dict1['Rload'] = '%11.4e' % (Rload)
    dict1['Ts'] = '%11.4e' % (Ts)
    dict1['dt'] = '%11.4e' % (dt)
    dict1['a0'] = '%11.4e' % (a0)
    dict1['a1'] = '%11.4e' % (a1)
    dict1['a2'] = '%11.4e' % (a2)
    dict1['b0'] = '%11.4e' % (b0)
    dict1['b1'] = '%11.4e' % (b1)
    dict1['b2'] = '%11.4e' % (b2)

    print('Rload:', Rload)

    print('dual_active_bridge_converter_parm: parameters computed')
