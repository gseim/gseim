def half_bridge_llc_converter_parm(dict1):
    import numpy as np

    V_out_desired = 12.0
    R1 = 19.7e3
    R2 = R1/3
    V_ref = V_out_desired*R2/(R1 + R2)

    wz1 = 2*np.pi*4.2e3
    wp0 = 3.2e3
    wp1 = 2*np.pi*8.2e3

    a1_1 = 1/wz1
    b1_1 = 1/wp1

    a0_2 = wp0

    k1 = R2/(R1 + R2)

    dict1['V_ref'] = '%11.4e' % (V_ref)
    dict1['a1_1'] = '%11.4e' % (a1_1)
    dict1['b1_1'] = '%11.4e' % (b1_1)
    dict1['a0_2'] = '%11.4e' % (a0_2)
    dict1['k1'] = '%11.4e' % (k1)

    print('half_bridge_llc_converter_parm: parameters computed')
