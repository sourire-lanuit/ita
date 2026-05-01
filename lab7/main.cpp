#include "Header.h" 
#include <iostream> 
#include <cmath>
#include <cstdint>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
static void set_utf8() { SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); }
#else
static void set_utf8() {}
#endif

int main() {
    set_utf8();
    std::mt19937 rng(42);

    std::vector<double> alphas;
    for (int i = 1; i <= 10; ++i) alphas.push_back(i * 0.05);

    constexpr int rep = 100;

    std::cout << "Таблиця залежності p_err від alpha та s\n";
    std::cout << "(усереднення по " << rep << " повторах)\n\n";

    for (double alpha : alphas) {
        int s0 = std::max(1, static_cast<int>(std::floor(std::log(2.0) / alpha)));
        std::vector<int> s_vals;
        if (s0 > 1) s_vals.push_back(s0 - 1);
        s_vals.push_back(s0);
        s_vals.push_back(s0 + 1);

        double perr_theory = std::pow(1.0 - std::exp(-alpha * s0), s0);

        std::cout << "alpha = " << std::fixed << std::setprecision(2) << alpha << "  (s0 = " << s0 << ", теоретична p_err ≈ " << std::scientific << std::setprecision(4) << perr_theory << ")\n";
        std::cout << std::string(55, '-') << "\n";
        std::cout << std::setw(6) << "s" << std::setw(18) << "p_err (експер.)" << std::setw(18) << "p_err (теорія)\n";

        for (int sv : s_vals) {
            double sum = 0.0;
            for (int r = 0; r < rep; ++r) sum += one_experiment(alpha, sv, rng);
            double perr_exp = sum / rep;
            double perr_th  = std::pow(1.0 - std::exp(-alpha * sv), sv);

            std::cout << std::setw(6) << sv << std::setw(18) << std::scientific << std::setprecision(4) << perr_exp << std::setw(18) << perr_th << "\n";
        }
        std::cout << "\n";
    }

    return 0;
}