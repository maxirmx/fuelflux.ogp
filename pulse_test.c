// pulse_test.c
// Simple pulse counter using libgpiod (v1 API).
//
// Build:  make
// Run:    ./pulse_test gpiochip0 72
//         ./pulse_test gpiochip0 72 --k 80
//
// Active-low pulses assumed -> falling edge.

#include <gpiod.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void usage(const char* prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s <gpiochip> <line_offset> [--k <pulses_per_liter>]\n\n"
        "Examples:\n"
        "  %s gpiochip0 72\n"
        "  %s gpiochip0 72 --k 80\n",
        prog, prog, prog
    );
}

int main(int argc, char** argv) {
    if (argc < 3) {
        usage(argv[0]);
        return 2;
    }

    const char* chipname = argv[1];
    unsigned int line_off = (unsigned int)strtoul(argv[2], NULL, 10);

    double K = 0.0;
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--k") == 0 && i + 1 < argc) {
            K = strtod(argv[i + 1], NULL);
            i++;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown arg: %s\n", argv[i]);
            usage(argv[0]);
            return 2;
        }
    }

    if (K < 0.0) {
        fprintf(stderr, "Invalid K\n");
        return 2;
    }

    struct gpiod_chip* chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        fprintf(stderr, "gpiod_chip_open_by_name(%s) failed: %s\n", chipname, strerror(errno));
        return 1;
    }

    struct gpiod_line* line = gpiod_chip_get_line(chip, line_off);
    if (!line) {
        fprintf(stderr, "gpiod_chip_get_line(%u) failed: %s\n", line_off, strerror(errno));
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_falling_edge_events(line, "pulse_test") < 0) {
        fprintf(stderr, "request_falling_edge_events failed: %s\n", strerror(errno));
        gpiod_chip_close(chip);
        return 1;
    }

    uint64_t pulses_total = 0;
    const double window_s = 1.0;
    double window_start = now_seconds();
    uint64_t window_pulses = 0;

    printf("Pulse test: chip=%s line=%u", chipname, line_off);
    if (K > 0.0) printf("  K=%.6f pulses/L", K);
    printf("\nCtrl+C to stop.\n");

    while (1) {
        struct timespec timeout = { .tv_sec = 2, .tv_nsec = 0 };
        int rc = gpiod_line_event_wait(line, &timeout);
        if (rc < 0) {
            fprintf(stderr, "event_wait failed: %s\n", strerror(errno));
            break;
        }

        double t = now_seconds();

        if (rc == 1) {
            struct gpiod_line_event ev;
            if (gpiod_line_event_read(line, &ev) < 0) {
                fprintf(stderr, "event_read failed: %s\n", strerror(errno));
                break;
            }
            pulses_total++;
            window_pulses++;
        }

        if ((t - window_start) >= window_s) {
            double elapsed = t - window_start;
            double pps = (elapsed > 0.0) ? ((double)window_pulses / elapsed) : 0.0;

            if (K > 0.0) {
                double liters = (double)pulses_total / K;
                double lpm = (pps * 60.0) / K;
                printf("pulses=%" PRIu64 "  liters=%.6f  flow=%.3f L/min\n",
                       pulses_total, liters, lpm);
            } else {
                printf("pulses=%" PRIu64 "  pps=%.2f\n", pulses_total, pps);
            }

            window_start = t;
            window_pulses = 0;
        }
    }

    gpiod_chip_close(chip);
    return 0;
}
