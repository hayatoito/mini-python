
def len(x):
    if bi_is_tuple(x):
        return bi_len_tuple(x)
    elif bi_is_list(x):
        return bi_len_list(x)
    elif bi_is_dict(x):
        return bi_len_dict(x)
    elif bi_is_string(x):
        return bi_len_string(x)
    else:
        type_error()

def values(d):
    if bi_is_dict(d):
        return bi_values_dict(d)
    else:
        type_error()

def keys(d):
    if bi_is_dict(d):
        return bi_keys_dict(d)
    else:
        type_error()

def items(d):
    if bi_is_dict(d):
        return bi_items_dict(d)
    else:
        type_error()

def append(l, x):
    bi_append_list(l, x)

def range(a, b):
    if a > b:
        fatal_error()
    l = []
    i = a
    while i < b:
        append(l, i)
        i = i + 1
    return l

def __repr__tuple(t):
    s = "("
    for i in range(0, len(t)):
        if i > 0:
            s = s + ", "
        s = s + __repr__(t[i])
    if len(t) == 1:
        s = s + ",)"
    else:
        s = s + ")"
    return s

def __repr__list(l):
    s = "["
    for i in range(0, len(l)):
        if i > 0:
            s = s + ", "
        s = s + __repr__(l[i])
    s = s + "]"
    return s

def __repr__dict(x):
    s = "{}"
    kvs = x.items()
    for i in range(0, len(kvs)):
        if i > 0:
            s = s + ", "
        s = s + __repr__(kvs[i][0]) + " : " + __repr__(kvs[i][1])
    s = s + "}"
    return s

def __repr__(x):
    if bi_is_int(x):
        return bi__repr__int(x)
    elif bi_is_float(x):
        return bi__repr__float(x)
    elif bi_is_tuple(x):
        return __repr__tuple(x)
    elif bi_is_list(x):
        return __repr__list(x)
    elif bi_is_dict(x):
        return __repr__dict(x)
    elif bi_is_string(x):
        return "\"" + x + "\""
    elif bi_is_ifun(x):
        return bi__repr__ifun(x)
    elif bi_is_nfun(x):
        return bi__repr__nfun(x)
    else:
        fatal_error()

def __str__(x):
    if bi_is_string(x):
        return x
    else:
        return __repr__(x)

def print_(x):
    bi_print_string(__str__(x))

def is_hashable(i):
    return 1

def getitem(a, i):
    if bi_is_list(a):
        if bi_is_int(i):
            return bi_getitem_list(a, i)
        else:
            type_error()
    elif bi_is_tuple(a):
        if bi_is_int(i):
            return bi_getitem_tuple(a, i)
        else:
            type_error()
    elif bi_is_dict(a):
        if is_hashable(i):
            return bi_getitem_dict(a, i)
        else:
            type_error()
    else:
        type_error()

def setitem(a, i, x):
    if bi_is_list(a):
        if bi_is_int(i):
            return bi_setitem_list(a, i, x)
        else:
            type_error()
    elif bi_is_dict(a):
        if is_hashable(i):
            return bi_setitem_dict(a, i, x)
        else:
            type_error()
    else:
        type_error()

def delitem(a, i):
    if bi_is_list(a):
        if bi_is_int(i):
            return bi_delitem_list(a, i)
        else:
            type_error()
    elif bi_is_dict(a):
        if is_hashable(i):
            return bi_delitem_dict(a, i)
        else:
            type_error()
    else:
        type_error()

def not_(x):
    if x is None or x is 0:
        return 1
    else:
        return 0

def contains(a, x):
    if bi_is_list(a) or bi_is_tuple(a):
        for y in a:
            if x == y:
                return 1
        return 0
    else:
        type_error()

def cmp_seq(a, b):
    n = min(len(a), len(b))
    for i in range(0, n):
        if a[i] < b[i]:
            return -1
        elif a[i] > b[i]:
            return 1
    return bi_cmp_int(len(a), len(b))

# int,float < string < tuple < list < dict < ifun < nfun
def cmp(a, b):
    if bi_is_int(a):
        if bi_is_int(b):
            return bi_cmp_int(a, b)
        elif bi_is_float(b):
            return bi_cmp_float(bi_itof(a), b)
        else:
            return -1
    elif bi_is_float(a):
        if bi_is_int(b):
            return bi_cmp_float(a, bi_itof(b))
        elif bi_is_float(b):
            return bi_cmp_float(a, b)
        else:
            return -1
    elif bi_is_string(a):
        if bi_is_int(b) or bi_is_float(b):
            return 1
        elif bi_is_string(b):
            return cmp_seq(a, b)
        else:
            return -1
    elif bi_is_tuple(a):
        if bi_is_int(b) or bi_is_float(b) or bi_is_string(b):
            return 1
        elif bi_is_tuple(b):
            return cmp_seq(a, b)
        else:
            return -1
    elif bi_is_list(a):
        if bi_is_int(b) or bi_is_float(b) or bi_is_string(b) or bi_is_tuple(b):
            return 1
        elif bi_is_list(b):
            return cmp_seq(a, b)
        else:
            return -1
    elif bi_is_dict(a):
        if bi_is_int(b) or bi_is_float(b) or bi_is_string(b) or bi_is_tuple(b) or bi_is_list(b):
            return 1
        elif bi_is_dict(b):
            return bi_cmp_addr(a, b)
        else:
            return -1
    elif bi_is_ifun(a):
        if bi_is_int(b) or bi_is_float(b) or bi_is_string(b) or bi_is_tuple(b) or bi_is_list(b) or bi_is_dict(b):
            return 1
        elif bi_is_ifun(b):
            return bi_cmp_addr(a, b)
        else:
            return -1
    elif bi_is_nfun(a):
        if bi_is_int(b) or bi_is_float(b) or bi_is_string(b) or bi_is_tuple(b) or bi_is_list(b) or bi_is_dict(b) or bi_is_ifun(b):
            return 1
        elif bi_is_nfun(b):
            return bi_cmp_addr(a, b)
        else:
            return -1
    else:
        fatal_error()

def eq(a, b):
    return bi_eq_int(cmp(a, b), 0)

def ne(a, b):
    return bi_ne_int(cmp(a, b), 0)

def gt(a, b):
    return bi_gt_int(cmp(a, b), 0)

def ge(a, b):
    return bi_ge_int(cmp(a, b), 0)

def lt(a, b):
    return bi_lt_int(cmp(a, b), 0)

def le(a, b):
    return bi_le_int(cmp(a, b), 0)

def add(a, b):
    # int + int
    # int + float
    # float + int
    # float + float
    # string + string
    # list + list
    # tuple + tuple
    if bi_is_int(a):
        if bi_is_int(b):
            return bi_add_int(a, b)
        elif bi_is_float(b):
            return bi_add_float(bi_itof(a), b)
        else:
            type_error()
    elif bi_is_float(a):
        if bi_is_int(b):
            return bi_add_float(a, bi_itof(b))
        elif bi_is_float(b):
            return bi_add_float(a, b)
        else:
            type_error()
    elif bi_is_string(a) and bi_is_string(b):
        return bi_add_string(a, b)
    elif bi_is_list(a) and bi_is_list(b):
        ab = []
        for x in a:
            ab.append(x)
        for x in b:
            ab.append(x)
        return ab
    elif bi_is_tuple(a) and bi_is_tuple(b):
        return bi_add_tuple(a, b)
    else:
        type_error()

def sub(a, b):
    if bi_is_int(a):
        if bi_is_int(b):
            return bi_sub_int(a, b)
        elif bi_is_float(b):
            return bi_sub_float(bi_itof(a), b)
        else:
            type_error()
    elif bi_is_float(a):
        if bi_is_int(b):
            return bi_sub_float(a, bi_itof(b))
        elif bi_is_float(b):
            return bi_sub_float(a, b)
        else:
            type_error()
    else:
        type_error()

def mul(a, b):
    if bi_is_int(a):
        if bi_is_int(b):
            return bi_mul_int(a, b)
        elif bi_is_float(b):
            return bi_mul_float(bi_itof(a), b)
        else:
            type_error()
    elif bi_is_float(a):
        if bi_is_int(b):
            return bi_mul_float(a, bi_itof(b))
        elif bi_is_float(b):
            return bi_mul_float(a, b)
        else:
            type_error()
    elif bi_is_string(a) and bi_is_int(b):
        return bi_mul_string_int(a, b)
    else:
        type_error()

def div(a, b):
    if bi_is_int(a):
        if bi_is_int(b):
            return bi_div_int(a, b)
        elif bi_is_float(b):
            return bi_div_float(bi_itof(a), b)
        else:
            type_error()
    elif bi_is_float(a):
        if bi_is_int(b):
            return bi_div_float(a, bi_itof(b))
        elif bi_is_float(b):
            return bi_div_float(a, b)
        else:
            type_error()
    else:
        type_error()

def convert_string_1(a, b):
    i = bi_find_string(a, "%s")
    if i == -1:
        string_conversion_error()
    return bi_getslice_string(a, 0, i) + __str__(b) + bi_getslice_string(a, i + 1, len(a))

def convert_string(a, b):
    s = a
    if bi_is_tuple(b):
        for x in b:
            s = convert_string_1(s, x)
        return s
    else:
        return convert_string_1(a, b)

def mod(a, b):
    if bi_is_int(a):
        if bi_is_int(b):
            return bi_mod_int(a, b)
        else:
            type_error()
    elif bi_is_string(a):
        return convert_string(a, b)
    else:
        type_error()

def invert(a):
    if bi_is_int(a):
        return bi_invert_int(a)
    else:
        type_error()

def lshift(a, b):
    if bi_is_int(a) and bi_is_int(b):
        return bi_lshift_int(a, b)
    else:
        type_error()

def rshift(a, b):
    if bi_is_int(a) and bi_is_int(b):
        return bi_rshift_int(a, b)
    else:
        type_error()

def xor(a, b):
    if bi_is_int(a) and bi_is_int(b):
        return bi_xor_int(a, b)
    else:
        type_error()

def and_(a, b):
    if bi_is_int(a) and bi_is_int(b):
        return bi_and_int(a, b)
    else:
        type_error()

def or_(a, b):
    if bi_is_int(a) and bi_is_int(b):
        return bi_or_int(a, b)
    else:
        type_error()



