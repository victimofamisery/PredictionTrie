#include "PredictionTrie.h"
#include "qsize.h"
#include "qmargins.h"
#include "qpoint.h"

#include <algorithm>

PredictionTrie::PredictionTrie()
{
    _root = new PredictionTrie::PredictionTrieNode;
    _root->count = 0u;
    _root->type = PredictionTrie::PredictionTrieNode::Type::Root;
}

PredictionTrie::~PredictionTrie()
{
    delete _root;
}

PredictionTrie::PredictionTrieNode::~PredictionTrieNode()
{
    for(auto&& child : children) {
        delete child.second;
    }
}

void PredictionTrie::insert(const std::string& word)
{
    auto* current = _root;
    for (auto letter : word)
    {
        if (isalpha(letter)) {
        auto foundIt = current->children.find(letter);
        if (foundIt == current->children.end())
        {
            auto [it, _] = current->children.emplace(letter, new PredictionTrieNode);
            foundIt = it;
        }
        current = foundIt->second;
        }
    }
    current->type = PredictionTrie::PredictionTrieNode::Type::Leaf;
    current->count += 1u;
}

void PredictionTrie::remove(const std::string& word)
{
    auto found = find(word);
    if (found && found->type == PredictionTrieNode::Type::Leaf) {
        return;
    }
    auto search = _root;
    std::pair<PredictionTrieNode*, std::string> last = {_root, word};
    auto deleteWord = word;
    for (auto letter : word) {
        if (search->type == PredictionTrieNode::Type::Leaf && search != found) {
            last.first = search;
            last.second = deleteWord;
        }
        deleteWord.erase(0, 1);
        search = search->children.find(letter)->second;
    }
    if (!search->children.empty()) {
        search->type = PredictionTrieNode::Type::Regular;
        search->count = 0u;
    }
    else {
        while (last.first != found) {
        auto link = last.first->children.find(deleteWord[0])->second;
        last.second.erase(0, 1);
        delete last.first;
        last.first = link;
        }
    }
}

const PredictionTrie::PredictionTrieNode* PredictionTrie::find(const std::string& word) const
{
    auto* current = _root;
    for (auto letter : word)
    {
        auto foundIt = current->children.find(letter);

        if (foundIt == current->children.end())
        {
            return nullptr;
        }

        current = foundIt->second;
    }

    return current;
}

bool PredictionTrie::isPresented(const std::string& word) const
{
    auto* found = find(word);

    return found && found->type == PredictionTrie::PredictionTrieNode::Type::Leaf;
}

std::vector<PredictionTrie::MatchedPair> PredictionTrie::allWordsStartedWith(const std::string& wordBegin) const
{
    auto* found = find(wordBegin);
    if (!found)
    {
        return {};
    }

    std::vector<MatchedPair> result;
    collectAllWordsStartedWith(wordBegin, found->children, result);

    return result;
}

void PredictionTrie::collectAllWordsStartedWith(
    const std::string& wordPart,
    const std::unordered_map<char, PredictionTrieNode*>& letterLayer,
    std::vector<MatchedPair>& result) const
{
    for (auto&& [letter, node] : letterLayer)
    {
        std::string word = wordPart + letter;
        if (node->type == PredictionTrie::PredictionTrieNode::Type::Leaf)
        {
            result.push_back(std::make_tuple(word, node->count));
        }

        collectAllWordsStartedWith(word, node->children, result);
    }
}

std::string PredictionTrie::findBestMatch(const std::string& wordBegin) const
{
    return findBestMatches(wordBegin, 1).front();
}

std::vector<std::string> PredictionTrie::findBestMatches(const std::string& wordBegin, unsigned count) const
{
    auto allWords = allWordsStartedWith(wordBegin);

    count = allWords.size() < count ? allWords.size() : count;

    std::partial_sort(
        std::begin(allWords),
        allWords.begin() + count,
        std::end(allWords),
        [](const auto& first, const auto& second) {
            return std::get<1>(first) > std::get<1>(second);
        }
    );
    std::vector<std::string> result;
    for(auto it = allWords.begin(); it < allWords.begin() + count; it++) {
        result.push_back(std::get<0>(*it));
    }
    return result;
}
