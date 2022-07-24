from .utils import Spec, expand, str_

PARAM_SCHEME = expand(
    base_key=str_,   # todo: rename/remove

    id=str_,
    label=str_,
    category=str_,

    dtype=str_,
    default=object,

    options=list,
    option_labels=list,
    option_attributes=Spec(types=dict, required=False, item_scheme=(str_, list)),

    hide=str_,
)
#PORT_SCHEME = expand(
#    label=str_,
#    domain=str_,
#
#    id=str_,
#    dtype=str_,
#    vlen=(int, str_),
#
#    multiplicity=(int, str_),
#    optional=(bool, int, str_),
#    hide=(bool, str_),
#)
PORT_SCHEME = expand(
    label=str_,
    domain=str_,

    id=str_,
    dtype=str_,

    hide=(bool, str_),
)
OUTVAR_SCHEME = expand(
    id=str_,
    outvars=list,
)

BLOCK_SCHEME = expand(
    id=Spec(types=str_, required=True, item_scheme=None),
    label=str_,
    category=str_,
    flags=(list, str_),

    parameters=Spec(types=list, required=False, item_scheme=PARAM_SCHEME),
    inputs=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    outputs=Spec(types=list, required=False, item_scheme=PORT_SCHEME),

    e_left_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    e_right_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    e_top_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    e_bottom_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),

    b_left_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    b_right_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    b_top_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),
    b_bottom_nodes=Spec(types=list, required=False, item_scheme=PORT_SCHEME),

    outvars=(list, str_),

    asserts=(list, str_),
    value=str_,

    documentation=str_,
    grc_source=str_,

    block_wrapper_path=str_,  # todo: rename/remove
)
