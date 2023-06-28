#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>

bool get_bit(uint16_t x, int idx) {
        return ((x & (1 << idx)) != 0);
}

uint8_t set_bit(uint8_t x, int idx, bool bit) {
        if (bit) {
                return (1 << idx) | x;
        } else {
                return (~(1 << idx)) & x;
        }
}

uint8_t manchester_decode_byte(uint16_t x) {
        uint8_t result = 0;

        for (int i = 0; i < 16; i += 2) {
                bool first = get_bit(x, i);
                bool second = get_bit(x, i + 1);

                if (!first && second) {
                        result = set_bit(result, i / 2, 1);
                } else if (first && !second) {
                        result = set_bit(result, i / 2, 0);
                } else {
                        errx(1, "invalid sequence");
                }
        }

        return result;
}

void manchester_decode_bytes(uint16_t* in_buf, uint8_t* out_buf, int n) {
        for (int i = 0; i < n; i++) {
                out_buf[i] = manchester_decode_byte(in_buf[i]);
        }
}

int main(int argc, char* argv[]) {
        if (argc != 3) {
                warnx("%s encodes a file to manchester encoding", argv[0]);
                errx(1, "usage: %s <input file> <output file>", argv[0]);
        }

        int fd_in = open(argv[1], O_RDONLY);
        if (fd_in < 0) {
                err(1, "could not open %s", argv[1]);
        }

        int fd_out = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (fd_out < 0) {
                err(1, "could not open %s", argv[1]);
        }

        uint16_t in_buf[4096];
        uint8_t out_buf[4096];

        int bytes_read;
        while ((bytes_read = read(fd_in, in_buf, sizeof(in_buf))) > 0) {
                if (bytes_read % 2 != 0) {
                        errx(1, "we read an odd number of bytes");
                }

                manchester_decode_bytes(in_buf, out_buf, bytes_read / 2);
                int bytes_written = write(fd_out, out_buf, bytes_read / 2);
                if (bytes_written < 0) {
                        err(1, "could not write data to %s", argv[2]);
                }
                if (bytes_read != bytes_written * 2) {
                        errx(1, "could not write all data at once");
                }
        }
        if (bytes_read < 0) {
                err(1, "could not read data from %s", argv[1]);
        }

        close(fd_in);
        close(fd_out);

        return 0;
}
v


/*
The Manchester decoding algorithm works by examining pairs of bits and determining the original binary value based on the transitions and levels of the Manchester-encoded signal. Here's a step-by-step explanation of how the Manchester decoding algorithm in the provided code works:

1. The input to the Manchester decoding algorithm is a 16-bit value `x` that represents a Manchester-encoded byte.

2. The algorithm iterates over the bits of `x` in pairs, starting from the least significant bit (LSB) position and moving towards the most significant bit (MSB) position.

3. For each pair of bits, the algorithm examines the transitions and levels of the Manchester-encoded signal to determine the original binary value.

4. The two bits in each pair are referred to as "first" and "second" bits.

5. The decoding algorithm considers the following conditions for each pair of bits:

   a. If the first bit is `0` and the second bit is `1`, it indicates a rising edge transition in the Manchester-encoded signal. This transition represents a binary `1` in the original data.

   b. If the first bit is `1` and the second bit is `0`, it indicates a falling edge transition in the Manchester-encoded signal. This transition represents a binary `0` in the original data.

   c. If neither of the above conditions is met, it means the pair of bits does not represent a valid Manchester encoding. In this case, the algorithm raises an error to indicate an invalid sequence.

6. Based on the above conditions, the algorithm reconstructs the original binary data by setting or clearing the corresponding bit in the output byte.

7. After processing all the bit pairs, the algorithm returns the resulting 8-bit value, which represents the decoded byte.

The `manchester_decode_byte` function in the provided code implements this Manchester decoding algorithm. It performs the above steps to decode a 16-bit Manchester-encoded value into an 8-bit result. The `manchester_decode_bytes` function extends this decoding process to handle multiple input values by calling `manchester_decode_byte` for each value.

It's important to note that the Manchester decoding algorithm assumes a specific encoding scheme where rising and falling edges are used to represent binary `1` and `0`, respectively. This encoding ensures reliable data transmission and synchronization between the sender and receiver over a communication channel.

Example:
Certainly! Let's consider an example to demonstrate the input and output of the Manchester decoding process.

Suppose we have a Manchester-encoded byte `0xC4`, which is represented by the 16-bit value `1100010000000000`. The Manchester encoding scheme follows the rule that a rising edge transition represents a `1` and a falling edge transition represents a `0`.

Input:
```
x = 0xC4 (binary: 1100010000000000)
```

To decode this Manchester-encoded byte, we'll apply the Manchester decoding algorithm step by step:

1. We start with the least significant bit (LSB) and move towards the most significant bit (MSB) in pairs.

2. First pair: `11`
   - The first bit is `1`, indicating a falling edge transition (0).
   - The second bit is `1`, indicating a rising edge transition (1).
   - Based on the Manchester encoding rules, this pair represents a binary `0`.

3. Second pair: `00`
   - The first bit is `0`, indicating a rising edge transition (1).
   - The second bit is `0`, indicating a falling edge transition (0).
   - Based on the Manchester encoding rules, this pair represents a binary `1`.

4. Third pair: `10`
   - The first bit is `1`, indicating a falling edge transition (0).
   - The second bit is `0`, indicating a falling edge transition (0).
   - Based on the Manchester encoding rules, this pair represents a binary `0`.

5. Fourth pair: `00`
   - The first bit is `0`, indicating a rising edge transition (1).
   - The second bit is `0`, indicating a falling edge transition (0).
   - Based on the Manchester encoding rules, this pair represents a binary `1`.

6. The Manchester decoding algorithm has processed all the bit pairs, and the resulting decoded byte is `0x50` (binary: 01010000).

Output:
```
Decoded byte: 0x50 (binary: 01010000)
```

So, in this example, the Manchester decoding process has transformed the Manchester-encoded byte `0xC4` into the decoded byte `0x50` by examining the transitions and levels of the Manchester-encoded signal.

*/