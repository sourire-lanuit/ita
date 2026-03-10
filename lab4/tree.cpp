#include "Header.h"

#include <queue>
#include <stack>

struct Cmp {
    bool operator()(const Huffman* a, const Huffman* b) const {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->symbol > b->symbol;
    }
};

Huffman* build_tree(const std::array<uint32_t, alph_size>& freqs)
{
    std::priority_queue<Huffman*, std::vector<Huffman*>, Cmp> pq;

    for (int i = 0; i < alph_size; ++i)
        if (freqs[i] > 0) pq.push(new Huffman(freqs[i], i));

    if (pq.empty()) return nullptr;

    if (pq.size() == 1) {
        Huffman* only = pq.top(); pq.pop();
        return new Huffman(only->freq, -1, only, nullptr);
    }

    while (pq.size() > 1) {
        Huffman* l = pq.top(); pq.pop();
        Huffman* r = pq.top(); pq.pop();
        pq.push(new Huffman(l->freq + r->freq, -1, l, r));
    }

    return pq.top();
}

void free_tree(Huffman* root)
{
    if (!root) return;
    std::stack<Huffman*> stk;
    stk.push(root);
    while (!stk.empty()) {
        Huffman* n = stk.top(); stk.pop();
        if (n->left)  stk.push(n->left);
        if (n->right) stk.push(n->right);
        delete n;
    }
}

void build_table(Huffman* root, the_table& table) {
    for (auto& e : table) { e.bits.clear(); e.length = 0; }
    if (!root) return;

    struct Frame {
        Huffman* node;
        std::vector<uint8_t> bits;
        int length;
    };

    std::stack<Frame> stk;
    stk.push({root, {}, 0});

    while (!stk.empty()) {
        auto [node, bits, length] = stk.top();
        stk.pop();

        if (!node->left && !node->right) {
            int sym = node->symbol;
            table[sym].bits = bits;
            table[sym].length = (length == 0) ? 1 : length;
            continue;
        }
        
        auto add_bit = [](std::vector<uint8_t> b, int len, int bit) -> std::pair<std::vector<uint8_t>, int> {
            int byte_ind = len / 8;
            int bit_ind = len % 8;
            if (bit_ind == 0) b.push_back(0);
            if (bit) b[byte_ind] |= (1 << bit_ind);
            return {b, len + 1};
        };

        if (node->right) {
            auto [nb, nl] = add_bit(bits, length, 1);
            stk.push({node->right, nb, nl});
        }
        if (node->left) {
            auto [nb, nl] = add_bit(bits, length, 0); 
            stk.push({node->left,  nb, nl});
        }
    }
}