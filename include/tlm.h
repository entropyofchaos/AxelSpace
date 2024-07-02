#ifndef TLM_H
#define TLM_H

#include <cstdint>

/**
 * Mock of CompoTlm_t struct. Expectation is this will
 * be replaced by real struct when integrated with main
 * project.
 */
typedef struct {
    uint8_t subsys_id;
    uint8_t compo_id;
    float temperature;
} CompoTlm_t;

#endif // TLM_H
