def s_gate_pulses_1ph_1_parm(dict1):
    y_high = float(dict1['y_high'])
    hb2 = 0.5*y_high
    dict1['hb2'] = '%11.4e' % (hb2)
    print('s_gate_pulses_1ph_1_parm: computed')
