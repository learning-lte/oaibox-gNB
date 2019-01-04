#define NR_MOD_TABLE_SIZE_SHORT 686
#define NR_MOD_TABLE_BPSK_OFFSET 1
#define NR_MOD_TABLE_QPSK_OFFSET 3
#define NR_MOD_TABLE_QAM16_OFFSET 7
#define NR_MOD_TABLE_QAM64_OFFSET 23
#define NR_MOD_TABLE_QAM256_OFFSET 87
short nr_mod_table[NR_MOD_TABLE_SIZE_SHORT] = {0,0,16384,16384,-16384,-16384,16384,16384,16384,-16384,-16384,16384,-16384,-16384,7327,7327,7327,21981,21981,7327,21981,21981,7327,-7327,7327,-21981,21981,-7327,21981,-21981,-7327,7327,-7327,21981,-21981,7327,-21981,21981,-7327,-7327,-7327,-21981,-21981,-7327,-21981,-21981,10726,10726,10726,3576,3576,10726,3576,3576,10726,17876,10726,25027,3576,17876,3576,25027,17876,10726,17876,3576,25027,10726,25027,3576,17876,17876,17876,25027,25027,17876,25027,25027,10726,-10726,10726,-3576,3576,-10726,3576,-3576,10726,-17876,10726,-25027,3576,-17876,3576,-25027,17876,-10726,17876,-3576,25027,-10726,25027,-3576,17876,-17876,17876,-25027,25027,-17876,25027,-25027,-10726,10726,-10726,3576,-3576,10726,-3576,3576,-10726,17876,-10726,25027,-3576,17876,-3576,25027,-17876,10726,-17876,3576,-25027,10726,-25027,3576,-17876,17876,-17876,25027,-25027,17876,-25027,25027,-10726,-10726,-10726,-3576,-3576,-10726,-3576,-3576,-10726,-17876,-10726,-25027,-3576,-17876,-3576,-25027,-17876,-10726,-17876,-3576,-25027,-10726,-25027,-3576,-17876,-17876,-17876,-25027,-25027,-17876,-25027,-25027,8886,8886,8886,12439,12439,8886,12439,12439,8886,5332,8886,1778,12439,5332,12439,1778,5332,8886,5332,12439,1778,8886,1778,12439,5332,5332,5332,1778,1778,5332,1778,1778,8886,19547,8886,15993,12439,19547,12439,15993,8886,23101,8886,26655,12439,23101,12439,26655,5332,19547,5332,15993,1778,19547,1778,15993,5332,23101,5332,26655,1778,23101,1778,26655,19547,8886,19547,12439,15993,8886,15993,12439,19547,5332,19547,1778,15993,5332,15993,1778,23101,8886,23101,12439,26655,8886,26655,12439,23101,5332,23101,1778,26655,5332,26655,1778,19547,19547,19547,15993,15993,19547,15993,15993,19547,23101,19547,26655,15993,23101,15993,26655,23101,19547,23101,15993,26655,19547,26655,15993,23101,23101,23101,26655,26655,23101,26655,26655,8886,-8886,8886,-12439,12439,-8886,12439,-12439,8886,-5332,8886,-1778,12439,-5332,12439,-1778,5332,-8886,5332,-12439,1778,-8886,1778,-12439,5332,-5332,5332,-1778,1778,-5332,1778,-1778,8886,-19547,8886,-15993,12439,-19547,12439,-15993,8886,-23101,8886,-26655,12439,-23101,12439,-26655,5332,-19547,5332,-15993,1778,-19547,1778,-15993,5332,-23101,5332,-26655,1778,-23101,1778,-26655,19547,-8886,19547,-12439,15993,-8886,15993,-12439,19547,-5332,19547,-1778,15993,-5332,15993,-1778,23101,-8886,23101,-12439,26655,-8886,26655,-12439,23101,-5332,23101,-1778,26655,-5332,26655,-1778,19547,-19547,19547,-15993,15993,-19547,15993,-15993,19547,-23101,19547,-26655,15993,-23101,15993,-26655,23101,-19547,23101,-15993,26655,-19547,26655,-15993,23101,-23101,23101,-26655,26655,-23101,26655,-26655,-8886,8886,-8886,12439,-12439,8886,-12439,12439,-8886,5332,-8886,1778,-12439,5332,-12439,1778,-5332,8886,-5332,12439,-1778,8886,-1778,12439,-5332,5332,-5332,1778,-1778,5332,-1778,1778,-8886,19547,-8886,15993,-12439,19547,-12439,15993,-8886,23101,-8886,26655,-12439,23101,-12439,26655,-5332,19547,-5332,15993,-1778,19547,-1778,15993,-5332,23101,-5332,26655,-1778,23101,-1778,26655,-19547,8886,-19547,12439,-15993,8886,-15993,12439,-19547,5332,-19547,1778,-15993,5332,-15993,1778,-23101,8886,-23101,12439,-26655,8886,-26655,12439,-23101,5332,-23101,1778,-26655,5332,-26655,1778,-19547,19547,-19547,15993,-15993,19547,-15993,15993,-19547,23101,-19547,26655,-15993,23101,-15993,26655,-23101,19547,-23101,15993,-26655,19547,-26655,15993,-23101,23101,-23101,26655,-26655,23101,-26655,26655,-8886,-8886,-8886,-12439,-12439,-8886,-12439,-12439,-8886,-5332,-8886,-1778,-12439,-5332,-12439,-1778,-5332,-8886,-5332,-12439,-1778,-8886,-1778,-12439,-5332,-5332,-5332,-1778,-1778,-5332,-1778,-1778,-8886,-19547,-8886,-15993,-12439,-19547,-12439,-15993,-8886,-23101,-8886,-26655,-12439,-23101,-12439,-26655,-5332,-19547,-5332,-15993,-1778,-19547,-1778,-15993,-5332,-23101,-5332,-26655,-1778,-23101,-1778,-26655,-19547,-8886,-19547,-12439,-15993,-8886,-15993,-12439,-19547,-5332,-19547,-1778,-15993,-5332,-15993,-1778,-23101,-8886,-23101,-12439,-26655,-8886,-26655,-12439,-23101,-5332,-23101,-1778,-26655,-5332,-26655,-1778,-19547,-19547,-19547,-15993,-15993,-19547,-15993,-15993,-19547,-23101,-19547,-26655,-15993,-23101,-15993,-26655,-23101,-19547,-23101,-15993,-26655,-19547,-26655,-15993,-23101,-23101,-23101,-26655,-26655,-23101,-26655,-26655};
