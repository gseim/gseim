def cyclo_converter_1ph_1_parm(dict1):
    f0 = float(dict1['f0'])

    f1 = f0/3

    dict1['f1'] = '%11.4e' % (f1)
    print('cyclo_converter_1ph_1_parm: parameters computed')
