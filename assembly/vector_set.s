.global vector_set
.args
    i32* src1 0 uav10 RW
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
    # Load src base addresses (arguments) from CB1
    s_buffer_load_dword s4, s[8:11], 0x00
    # Load UAVs from UAV table
    s_load_dwordx4 s[20:23], s[2:3], 0x50
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
    # Load src1[id] and src2[id]
    tbuffer_load_format_x v20, v10, s[20:23], 0 offen format:[BUF_DATA_FORMAT_32, BUF_NUM_FORMAT_FLOAT]
    # Waits for memory operations to complete
    s_waitcnt vmcnt(0)

    # Compare the gid[id] to 32.
    v_cmp_lt_i32 s[16:17], v2, 32

    # Set the exec register to exec & s[16:17]
    s_and_saveexec_b64 s[18:19], s[16:17]

    # If the none of the workers will execute the first part of
    # the conditional, branch to the second case.
    s_cbranch_execz else

    # Set source elements = -gid[id].
    v_sub_i32 v22, vcc, 0, v2

    # End here because we'll never have
    # partial wavefronts in this condition.

    # Store result in dst[id]
    tbuffer_store_format_x v22, v10, s[20:23], 0 offen format:[BUF_DATA_FORMAT_32,BUF_NUM_FORMAT_FLOAT]

    # End program
    s_endpgm

else:
    # "Flip" (& ~) the exec register.
    s_andn2_b64 exec, s[18:19], exec

    # x = 0
    v_mov_b32 v22, 0

    # i = 0
    v_mov_b32 v23, 0

for_start:
    # i < gid[id]
    v_cmp_lt_i32 s[16:17], v23, v2

    # Set the exec register.
    s_and_saveexec_b64 s[18:19], s[16:17]

    # Finish if there are no more WI in the loop.
    s_cbranch_execz for_end

    # Increment x
    v_add_i32 v22, vcc, 1, v22

    # Increment i
    v_add_i32 v23, vcc, 1, v23

    # Go back to the top.
    s_cbranch_execnz for_start

for_end:
    s_mov_b64 exec, s[18:19]

    # Store result in dst[id]
    tbuffer_store_format_x v22, v10, s[20:23], 0 offen format:[BUF_DATA_FORMAT_32,BUF_NUM_FORMAT_FLOAT]

    # End program
    s_endpgm

