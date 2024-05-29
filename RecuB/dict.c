#include <assert.h>
#include <stdlib.h>
#include "dict.h"
#include "string.h"

struct _node_t {
    string word;
    string definition;
    struct _node_t *next;
};

struct _dict_t {
    node_t words;
    unsigned int size;
};
// ^-- es una deficion nos compleja 

static node_t create_node(string word, string def) {
    node_t node = malloc(sizeof(struct _node_t));
    if (node == NULL) {
        perror("Unable to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    node->word = word;
    node->definition = def;
    node->next = NULL;
    return node;
}

static node_t destroy_node(node_t node) {
    if (node != NULL) {
        string_destroy(node->word);
        string_destroy(node->definition);
        free(node);
    }
    return NULL;
}

static bool invrep(dict_t dict) {
    if (dict == NULL) return false;

    node_t current = dict->words;
    unsigned int count = 0;

    while (current != NULL) {
        if (current->word == NULL || current->definition == NULL) return false;
        current = current->next;
        count++;
    }

    return (count == dict->size);
}
//esto simplifica la definicion de invrep

dict_t dict_empty(void) {
    dict_t dict = malloc(sizeof(struct _dict_t));
    if (dict == NULL) {
        perror("Unable to allocate memory for dictionary");
        exit(EXIT_FAILURE);
    }
    dict->words = NULL;
    dict->size = 0;
    assert(invrep(dict));
    return dict;
}

static node_t remove_on_index(node_t list, int index) {
    if (index == 0) {
        node_t temp = list;
        list = list->next;
        destroy_node(temp);
    } else {
        node_t prev = list;
        for (int i = 0; i < index - 1; i++) {
            prev = prev->next;
        }
        node_t temp = prev->next;
        prev->next = temp->next;
        destroy_node(temp);
    }
    return list;
}

static int find_index_of_key(dict_t dict, string word) {
    int index = 0;
    node_t current = dict->words;

    while (current != NULL) {
        if (string_eq(current->word, word)) {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;
}

dict_t dict_add(dict_t dict, string word, string def) {
    assert(invrep(dict));

    int index = find_index_of_key(dict, word);
    if (index != -1) {
        node_t current = dict->words;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        string_destroy(current->definition);
        current->definition = def;
    } else {
        node_t new_node = create_node(word, def);
        if (dict->words == NULL || string_less(word, dict->words->word)) {
            new_node->next = dict->words;
            dict->words = new_node;
        } else {
            node_t current = dict->words;
            while (current->next != NULL && string_less(current->next->word, word)) {
                current = current->next;
            }
            new_node->next = current->next;
            current->next = new_node;
        }
        dict->size++;
    }

    assert(invrep(dict));
    return dict;
}

string dict_search(dict_t dict, string word) {
    assert(invrep(dict));

    node_t current = dict->words;
    while (current != NULL) {
        if (string_eq(current->word, word)) {
            return current->definition;
        }
        current = current->next;
    }
    return NULL;
}

bool dict_exists(dict_t dict, string word) {
    assert(invrep(dict));
    return dict_search(dict, word) != NULL;
}

unsigned int dict_length(dict_t dict) {
    assert(invrep(dict));
    return dict->size;
}

dict_t dict_remove(dict_t dict, string word) {
    assert(invrep(dict));

    int index = find_index_of_key(dict, word);
    if (index != -1) {
        dict->words = remove_on_index(dict->words, index);
        dict->size--;
    }

    assert(invrep(dict));
    return dict;
}

dict_t dict_remove_all(dict_t dict) {
    assert(invrep(dict));

    while (dict->words != NULL) {
        dict->words = remove_on_index(dict->words, 0);
    }
    dict->size = 0;

    assert(invrep(dict));
    return dict;
}

void dict_dump(dict_t dict, FILE *file) {
    assert(invrep(dict));

    node_t current = dict->words;
    while (current != NULL) {
        fprintf(file, "%s : %s\n", string_ref(current->word), string_ref(current->definition));
        current = current->next;
    }
}

dict_t dict_destroy(dict_t dict) {
    dict = dict_remove_all(dict);
    free(dict);
    dict = NULL;
    return dict;
}
//end//
