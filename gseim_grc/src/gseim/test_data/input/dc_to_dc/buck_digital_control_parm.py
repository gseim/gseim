def buck_digital_control_parm(dict1):
    import numpy as np

    T = float(dict1['T'])

    Ki = 2*np.pi*500
    fz = 2*np.pi*1.7e3
    fp = 2*np.pi*14.5e3

##  Note: gseim and plecs xfer fn formats are different

    a0_1 = (1.0/(T*fp)) + 1.0
    a1_1 = -1.0/(T*fp)
    b0_1 = (1.0/(T*fz)) + 1.0
    b1_1 = -1.0/(T*fz)

    dict1['Ki'] = '%11.4e' % (Ki)
    dict1['a0_1'] = '%11.4e' % (a0_1)
    dict1['a1_1'] = '%11.4e' % (a1_1)
    dict1['b0_1'] = '%11.4e' % (b0_1)
    dict1['b1_1'] = '%11.4e' % (b1_1)

    print('buck_digital_control_parm: parameters computed')
