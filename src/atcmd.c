/**
 * @file atcmd.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <atcmd.h>
#include <string.h>

#define UNUSED __attribute__((unused))

void atcmd_parser_init(UNUSED atcmd_parser_t *parser, UNUSED const atcmd_t *atcmds, UNUSED uint32_t num_atcmds) {

}

uint8_t atcmd_detect_escape(UNUSED atcmd_parser_t *parser, UNUSED char c) {
    return 0;
}

uint8_t atcmd_parse(UNUSED atcmd_parser_t *parser, UNUSED char *cmd) {
    return 0;
}