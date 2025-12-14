#include <iostream>
#include <string>

int calc_r_from_n(int n){
    int r = 0;
    while ((1 << r) < n + 1){
        ++r;
    }
    return r;
}

bool check_hamming_params(int m, int n){
    int r = n - m;
    if (r <= 0) return false;
    return (1 << r) >= n + 1;
}

bool is_binary_string(const std::string& s){
    for (char c : s){
        if (c != '0' && c != '1') return false;
    }
    return true;
}

bool hamming_encode_any(const std::string& bin_vec, int m, int n, std::string& out){
    if ((int)bin_vec.size() != m) return false;
    if (!is_binary_string(bin_vec)) return false;
    if (!check_hamming_params(m, n)) return false;

    int r = n - m;
    out.assign(n, '0');

    int data_idx = 0;
    for (int pos = 1; pos <= n; ++pos){
        bool is_parity_pos = (pos & (pos - 1)) == 0;
        if (!is_parity_pos){
            char bit = bin_vec[data_idx++];
            out[pos - 1] = bit;
        }
    }

    for (int k = 0; k < r; ++k){
        int p = 1 << k;
        int parity = 0;
        for (int pos = 1; pos <= n; ++pos){
            if (pos & p){
                if (out[pos - 1] == '1'){
                    parity ^= 1;
                }
            }
        }
        out[p - 1] = (parity ? '1' : '0');
    }

    return true;
}

bool hamming_decode_any(const std::string& code_in, int n, int& r_out, std::string& data_out, bool& had_error, bool& uncorrectable){
    if ((int)code_in.size() != n) return false;
    if (!is_binary_string(code_in)) return false;

    int r = calc_r_from_n(n);
    int m = n - r;
    if (!check_hamming_params(m, n)){
        return false;
    }

    r_out = r;
    std::string code = code_in;
    had_error = false;
    uncorrectable = false;

    int syndrome = 0;
    for (int k = 0; k < r; ++k){
        int p = 1 << k;
        int parity = 0;
        for (int pos = 1; pos <= n; ++pos){
            if (pos & p){
                if (code[pos - 1] == '1'){
                    parity ^= 1;
                }
            }
        }
        if (parity){
            syndrome |= p;
        }
    }

    if (syndrome != 0){
        had_error = true;
        if (syndrome >= 1 && syndrome <= n){
            int idx = syndrome - 1;
            code[idx] = (code[idx] == '1' ? '0' : '1');
        } else {
            uncorrectable = true;
        }
    }

    data_out.clear();
    for (int pos = 1; pos <= n; ++pos){
        bool is_parity_pos = (pos & (pos - 1)) == 0;
        if (!is_parity_pos){
            data_out.push_back(code[pos - 1]);
        }
    }

    return true;
}

bool hamming_is_valid_any(const std::string& code_in, int n){
    if ((int)code_in.size() != n) return false;
    if (!is_binary_string(code_in)) return false;

    int r = calc_r_from_n(n);
    int m = n - r;
    if (!check_hamming_params(m, n)) return false;

    int syndrome = 0;
    for (int k = 0; k < r; ++k){
        int p = 1 << k;
        int parity = 0;
        for (int pos = 1; pos <= n; ++pos){
            if (pos & p){
                if (code_in[pos - 1] == '1'){
                    parity ^= 1;
                }
            }
        }
        if (parity){
            syndrome |= p;
        }
    }
    return syndrome == 0;
}

int main(int argc, char* argv[]){
    if (argc < 2){
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "encode"){
        if (argc != 5){
            return 1;
        }
        int m = 0;
        int n = 0;
        m = std::stoi(argv[2]);
        n = std::stoi(argv[3]);
        std::string bin_vec = argv[4];

        std::string code;
        if (!hamming_encode_any(bin_vec, m, n, code)){
            return 1;
        }
        std::cout << code << std::endl;
        return 0;
    }
    else if (cmd == "decode"){
        if (argc != 4){
            return 1;
        }
        int n = 0;
        n = std::stoi(argv[2]);
        std::string code = argv[3];

        int r = 0;
        std::string data;
        bool had_error = false;
        bool uncorrectable = false;
        if (!hamming_decode_any(code, n, r, data, had_error, uncorrectable)){
            return 1;
        }

        std::cout << r << std::endl;
        std::cout << data << std::endl;
        return 0;
    }
    else if (cmd == "is_valid"){
        if (argc != 4){
            return 1;
        }
        int n = 0;
        n = std::stoi(argv[2]);
    
        std::string code = argv[3];

        bool ok = hamming_is_valid_any(code, n);
        std::cout << (ok ? 1 : 0) << std::endl;
        return 0;
    }

    return 1;
}
