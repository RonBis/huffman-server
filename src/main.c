#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define none -1

typedef struct msgchar {
  char ch;
  int freq;
  int code; // initialized with none(-1)
} msgchar;

typedef struct htree_node {
  char assoc_ch; // associated character with frequency (-1 unless leaf node)
  int freq;
  struct htree_node *left_child;
  struct htree_node *right_child;
} hnode;

msgchar *chars;
int char_count;
hnode *root;

hnode *new_hnode(char assoc_ch, int freq, hnode *left_child,
                 hnode *right_child) {
  hnode *node = (hnode *)malloc(sizeof(hnode));
  node->assoc_ch = assoc_ch, node->freq = freq, node->left_child = left_child,
  node->right_child = right_child;
  return node;
}

/// @brief Generates the sorted frequency table of the characters in the
/// message.
/// @param msg
/// @param chars will hold the distinct characters in the message along with
/// their frequency
/// @return The length of distinct characters found in the message.
void generate_freq_table(char *msg) {
  // assume all characters are distinct
  // later realloc() to necessary amount
  char *found_chars = malloc(255 * sizeof(char));
  int found_chars_len = 1;
  found_chars[0] = msg[0];

  chars = (msgchar *)malloc(sizeof(msgchar));
  chars[0] = (msgchar){.ch = msg[0], .freq = 0, .code = none};

  for (int i = 0; msg[i] != '\0'; i++) {
    // checks if character is already found in msg or not
    bool found_new = true;
    for (int j = 0; j < found_chars_len; j++) {
      if (msg[i] == found_chars[j]) {
        found_new = false;

        // increase frequency by 1
        chars[j] =
            (msgchar){.ch = msg[i], .freq = chars[j].freq + 1, .code = none};
        break;
      }
    }
    if (found_new) {
      found_chars_len++;
      found_chars[found_chars_len - 1] = msg[i];

      chars = (msgchar *)realloc(chars, found_chars_len * sizeof(msgchar));
      chars[found_chars_len - 1] =
          (msgchar){.ch = msg[i], .freq = 1, .code = none};
    }
  }

  // sort frequency table
  for (int i = 0; i < found_chars_len; i++) {
    for (int j = 0; j < found_chars_len; j++) {
      if (chars[i].freq < chars[j].freq) {
        // swap
        msgchar tmp = chars[i];
        chars[i] = chars[j];
        chars[j] = tmp;
      }
    }
  }
  char_count = found_chars_len;
  free(found_chars);
}

void generate_tree() {
  hnode *leaf1 = new_hnode(chars[0].ch, chars[0].freq, NULL, NULL);
  hnode *leaf2 = new_hnode(chars[1].ch, chars[1].freq, NULL, NULL);
  hnode *parent = new_hnode(none, leaf1->freq + leaf2->freq, leaf1, leaf2);

  int count = 2;
  while (count != char_count) {
    leaf2 = new_hnode(chars[count].ch, chars[count].freq, NULL, NULL);
    if (parent->freq <= chars[count].freq) {
      parent = new_hnode(none, parent->freq + chars[count].freq, parent, leaf2);
    } else {
      parent = new_hnode(none, parent->freq + chars[count].freq, leaf2, parent);
    }

    count++;
  }
  root = parent;
}

void generate_code(char c, int path_string) {
  for (int i = 0; i < char_count; i++) {
    if(c == chars[i].ch) {
      chars[i].code = path_string;
    }
  }
}

void traverse_htree(hnode *node, int path_string) {
  // if null, then it is a leaf node which has an associated char(assoc_ch) with it
  if (node->left_child == NULL) {
    generate_code(node->assoc_ch, path_string);
    return;
  } else if (node->right_child == NULL) {
    generate_code(node->assoc_ch, path_string);
    return;
  } else {
    traverse_htree(node->left_child, path_string * 10 + 0);
    traverse_htree(node->right_child, path_string * 10 + 1);
  }
}

int main(int argv, char **argc) {
  int msg_len = strlen(argc[1]);
  // sample message: bcaadddccacacac
  generate_freq_table(argc[1]);
  generate_tree();

  // printf("\n## huffman codes ##\n\nchar\tfreq\tcode\n");
  traverse_htree(root, 0);
  for (int i = 0; i < char_count; i++) {
    printf("%c %d %d\n", chars[i].ch, chars[i].freq, chars[i].code);
  }

  return 0;
}
