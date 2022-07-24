def s_indmc_parm(dict1):

    j = float(dict1['j'])
    llr = float(dict1['llr'])
    lls = float(dict1['lls'])
    lm = float(dict1['lm'])
    rr = float(dict1['rr'])
    rs = float(dict1['rs'])
    poles = float(dict1['poles'])

    ls = lls + lm
    lr = llr + lm
    le = (ls*lr/lm) - lm
    l1 = lr/(lm*le)
    l2 = 1.0 + (lls/lm)
    l3 = lls/lm
    x1 = 0.75*poles*lm
    x2 = 0.5*poles

    i1_k = 1.0/j

    s1_k1 = 1.0
    s1_k2 = -rs

    s2_k1 = 1.0
    s2_k2 = -rs

    s3_k1 = -rr
    s3_k2 = -1.0

    s4_k1 = -rr
    s4_k2 =  1.0

    s6_k1 = l1
    s6_k2 = -1.0/le

    s7_k1 = l1
    s7_k2 = -1.0/le

    s8_k1 = 1.0/lm
    s8_k2 = -l2

    s9_k1 = 1.0/lm
    s9_k2 = -l2

    m1_k = x1

    dict1['i1_k'] = '%14.7e' % (i1_k)
    dict1['s1_k1'] = '%14.7e' % (s1_k1)
    dict1['s1_k2'] = '%14.7e' % (s1_k2)
    dict1['s2_k1'] = '%14.7e' % (s2_k1)
    dict1['s2_k2'] = '%14.7e' % (s2_k2)
    dict1['s3_k1'] = '%14.7e' % (s3_k1)
    dict1['s3_k2'] = '%14.7e' % (s3_k2)
    dict1['s4_k1'] = '%14.7e' % (s4_k1)
    dict1['s4_k2'] = '%14.7e' % (s4_k2)
    dict1['s6_k1'] = '%14.7e' % (s6_k1)
    dict1['s6_k2'] = '%14.7e' % (s6_k2)
    dict1['s7_k1'] = '%14.7e' % (s7_k1)
    dict1['s7_k2'] = '%14.7e' % (s7_k2)
    dict1['s8_k1'] = '%14.7e' % (s8_k1)
    dict1['s8_k2'] = '%14.7e' % (s8_k2)
    dict1['s9_k1'] = '%14.7e' % (s9_k1)
    dict1['s9_k2'] = '%14.7e' % (s9_k2)
    dict1['m1_k'] = '%14.7e' % (m1_k)
    dict1['x2'] = '%14.7e' % (x2)

    print('s_indmc: parameters computed')
