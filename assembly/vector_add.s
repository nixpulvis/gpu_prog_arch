.global vector_add
.args
    i32* src1 0 uav10 RO
    i32* src2 16 uav11 RO
    i32* dst 32 uav12 RW
.metadata
    uavprivate = 0
    hwregion = 0
    hwlocal = 0
    FloatMode = 192
    IeeeMode = 0
    # Loads UAV table in s2...s3
    userElements[0] = PTR_UAV_TABLE, 0, s[2:3]
    # Loads constant buffer 0 descriptor in s4...s7
    userElements[1] = IMM_CONST_BUFFER, 0, s[4:7]
    # Loads constant buffer 1 descriptor in s8...s11
    userElements[2] = IMM_CONST_BUFFER, 1, s[8:11]
    # Forces wg_id[0] (work-group ID in dimension 0) to be available in s12
    COMPUTE_PGM_RSRC2:USER_SGPR = 12
    COMPUTE_PGM_RSRC2:TGID_X_EN = 1
.text
    # Load lsize[0] into s0
    s_buffer_load_dword s0, s[4:7], 0x04
    # Load src1, src2, and dst base addresses (arguments) from CB1
    s_buffer_load_dword s4, s[8:11], 0x00
    s_buffer_load_dword s5, s[8:11], 0x04
    s_buffer_load_dword s6, s[8:11], 0x08
    # Load UAVs from UAV table
    s_load_dwordx4 s[20:23], s[2:3], 0x50
    s_load_dwordx4 s[24:27], s[2:3], 0x58
    s_load_dwordx4 s[28:31], s[2:3], 0x60
    # Waits for memory operations to complete
    s_waitcnt lgkmcnt(0)
    # v1 <= lsize[0]
    v_mov_b32 v1, s0
    # v1 <= lsize[0] * wg_id[0]
    v_mul_i32_i24 v1, s12, v1
    # v2 <= lsize[0] * wg_id[0] + lid[0] = gid[0]
    v_add_i32 v2, vcc, v0, v1
    # v3 <= gid[0] * 4
    v_lshlrev_b32 v3, 2, v2
    # Calcaulte effective addresses
    v_add_i32 v10, vcc, s4, v3
    v_add_i32 v11, vcc, s5, v3
    v_add_i32 v12, vcc, s6, v3
    # Load src1[id] and src2[id]
    tbuffer_load_format_x v20, v10, s[20:23], 0 offen format:[BUF_DATA_FORMAT_32, BUF_NUM_FORMAT_FLOAT]
    tbuffer_load_format_x v21, v11, s[24:27], 0 offen format:[BUF_DATA_FORMAT_32,BUF_NUM_FORMAT_FLOAT]
    # Waits for memory operations to complete
    s_waitcnt vmcnt(0)
    # Add source elements
    v_add_i32 v22, vcc, v20, v21
    # Store result in dst[id]
    tbuffer_store_format_x v22, v12, s[28:31], 0 offen format:[BUF_DATA_FORMAT_32,BUF_NUM_FORMAT_FLOAT]
    # End program
    s_endpgm
