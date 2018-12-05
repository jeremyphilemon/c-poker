#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct card {
  char face;
  char type;
  int value;
};

struct flags {
  int op;
  int tp;
  int toak;
  int foak;
  int fh;
  int flush_num;
  int straight;
  int sf;
  int rf;
  char flush;
};

struct player {
  int id;
  struct card hand[2];
  struct card full[7];
  struct card pairs[4];
  struct card final[5];
  struct flags flags;
  int checked;
  int rank;
} p[3];

struct table {
  struct card community[5];
} t;

void qna(struct player* current, struct table* t, int id) {
  current->id = id + 1;
  current->full[0] = current->hand[0];
  current->full[1] = current->hand[1];
  for (int i = 2; i < 7; i++) {
    current->full[i] = t->community[i - 2];
  }
  current->rank = -100;
  current->checked = -1;
}

void quantify(struct card* current) {
  if (current->face == 'A') {
    current->value = 14;
  } else if (current->face == 'K') {
    current->value = 13;
  } else if (current->face == 'Q') {
    current->value = 12;
  } else if (current->face == 'J') {
    current->value = 11;
  } else if (current->face == 'T') {
    current->value = 10;
  } else if (current->face >= '2' && current->face <= '9') {
    current->value = current->face - '0';
  }
}

void display_table(struct table current) {
  printf("Table: ");
  for (int i = 0; i < 5; i++) {
    printf("%d%c ", current.community[i].value, current.community[i].type);
  }
}

void display(struct player current) {
  struct card hand1 = current.hand[0];
  struct card hand2 = current.hand[1];
  printf("\n\nPlayer %d: %c%c %c%c > %d%c %d%c\n", current.id, hand1.face,
         hand1.type, hand2.face, hand2.type, hand1.value, hand1.type,
         hand2.value, hand2.type);
  printf("\tPairs: [%d, %d, %d, %d]", current.pairs[0].value,
         current.pairs[1].value, current.pairs[2].value,
         current.pairs[3].value);
  printf("\n\tOne Pair: %d", current.flags.op);
  printf("\n\tTwo Pair: %d", current.flags.tp);
  printf("\n\tThree of a Kind: %d", current.flags.toak);
  printf("\n\tStraight: %d", current.flags.straight);
  printf("\n\tFlush: %c", current.flags.flush);
  printf("\n\tFull House: %d", current.flags.fh);
  printf("\n\tFour of a Kind: %d", current.flags.foak);
  printf("\n\tStraight Flush: %d", current.flags.sf);
  printf("\n\tRoyal Flush: %d", current.flags.rf);
  printf("\n\t>Score: %d", current.rank);
}

void assign(char* line) {
  char whitespace = (char)32;
  sscanf(
      line, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
      &p[0].hand[0].face, &p[0].hand[0].type, &whitespace, &p[0].hand[1].face,
      &p[0].hand[1].type, &whitespace, &p[1].hand[0].face, &p[1].hand[0].type,
      &whitespace, &p[1].hand[1].face, &p[1].hand[1].type, &whitespace,
      &p[2].hand[0].face, &p[2].hand[0].type, &whitespace, &p[2].hand[1].face,
      &p[2].hand[1].type, &whitespace, &t.community[0].face,
      &t.community[0].type, &whitespace, &t.community[1].face,
      &t.community[1].type, &whitespace, &t.community[2].face,
      &t.community[2].type, &whitespace, &t.community[3].face,
      &t.community[3].type, &whitespace, &t.community[4].face,
      &t.community[4].type);
}

void setflags(struct player* current, int i) {
  current->id = i + 1;
  current->flags.op = -1;
  current->flags.tp = -1;
  current->flags.toak = -1;
  current->flags.foak = -1;
  current->flags.fh = -1;
  current->flags.straight = -1;
  current->flags.sf = -1;
  current->flags.rf = -1;
  current->flags.flush_num = -1;
  current->flags.flush = '\0';
}

void setflags2(struct player* current) {
  current->flags.op = -1;
  current->flags.tp = -1;
  current->flags.toak = -1;
  current->flags.foak = -1;
  current->flags.fh = -1;
}

struct card* getcards(int size) {
  struct card* cards = (struct card*)malloc(sizeof(struct card) * size);
  for (int i = 0; i < size; i++) {
    cards[i].value = -1;
    cards[i].type = '\0';
    cards[i].face = '\0';
  }
  return cards;
}

struct card* order(struct card* acards, int size, char sort) {
  struct card* cards = (struct card*)malloc(sizeof(struct card) * size);
  cards = acards;
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (sort == 'a') {
        if (cards[i].value > cards[j].value) {
          struct card temp = cards[i];
          cards[i] = cards[j];
          cards[j] = temp;
        }
      } else if (sort == 'd') {
        if (cards[i].value < cards[j].value) {
          struct card temp = cards[i];
          cards[i] = cards[j];
          cards[j] = temp;
        }
      }
    }
  }
  return cards;
}

struct card* straight(struct player* current) {
  struct card* cards = getcards(7);
  cards = order(current->full, 7, 'a');
  struct card* cleaned = getcards(7);
  struct card* straight = getcards(5);
  cleaned[0] = cards[0];
  struct card top = cards[0];
  int index = 1;
  for (int i = 1; i < 7; i++) {
    if (cards[i].value != top.value) {
      cleaned[index] = cards[i];
      top = cards[i];
      index++;
    }
  }
  cleaned = order(cleaned, index, 'd');
  int new_length = index, count = 1, max = 0;
  for (int i = 1; i < new_length; i++) {
    if (cleaned[i].value != -1) {
      if (cleaned[i].value != (cleaned[i - 1].value) - 1 && count < 5) {
        count = 1;
        continue;
      } else if (count == 5) {
        break;
      } else if (cleaned[i].value == (cleaned[i - 1].value) - 1) {
        if (count == 1) {
          max = cleaned[i - 1].value;
        }
        straight[count] = cleaned[i];
        count++;
      }
    }
  }
  straight[0].value = max;
  cleaned = order(cleaned, new_length, 'a');

  if (cleaned[0].value == 2 && cleaned[1].value == 3 && cleaned[2].value == 4 &&
      cleaned[3].value == 5 && cleaned[new_length - 1].value == 14 &&
      cleaned[4].value != 6) {
    current->flags.straight = 5;
  } else if (cleaned[0].value == 2 && cleaned[1].value == 3 &&
             cleaned[2].value == 4 && cleaned[3].value == 5 &&
             cleaned[new_length - 1].value == 14 && cleaned[4].value == 6) {
    current->flags.straight = 6;
  } else if (count >= 5) {
    current->flags.straight = max;
  }
  return straight;
}

void findpairs(struct player* current) {
  struct card* pairs = getcards(4);
  struct card* cards = getcards(7);
  int index = 0;
  cards = order(current->full, 7, 'a');
  for (int i = 0; i < 6; i++) {
    if (cards[i].value == cards[i + 1].value) {
      pairs[index] = cards[i];
      index++;
    }
  }
  pairs = order(pairs, 4, 'd');
  memcpy(current->pairs, (void*)pairs, sizeof(struct card) * 4);
}

struct card* fill(struct player* current) {
  struct card* cardsfull = getcards(7);
  cardsfull = order(current->full, 7, 'd');
  struct card* cards = getcards(4);
  cards = order(current->pairs, 4, 'd');
  int length = 0, index = 0;
  for (int i = 0; i < 4; i++) {
    if (cards[i].value != -1) {
      length++;
    }
  }
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 7; j++) {
      if (cards[i].value == cardsfull[j].value) {
        cardsfull[j].value = -1;
      }
    }
  }
  cardsfull = order(cardsfull, 7, 'd');
  struct card* copy = getcards(5);
  for (int i = 0; i < length; i++) {
    copy[i] = cards[i];
  }
  for (int i = length; i < 4; i++) {
    copy[i] = cardsfull[index];
    index++;
  }

  if (current->flags.fh > 0) {
    struct card* fix = getcards(5);
    fix[0] = copy[3];
    fix[1] = copy[4];
    fix = order(fix, 5, 'd');
    for (int i = 0; i < 3; i++) {
      for (int j = 4; j > 0; j--) {
        fix[j] = fix[j - 1];
      }
    }
    fix[0] = copy[0];
    fix[1] = copy[1];
    fix[2] = copy[2];
    copy = fix;
  } else if (current->flags.op > 0 && current->flags.tp > 0 &&
             current->pairs[2].value != -1) {
    struct card* fix = getcards(5);
    if (length == 2) {
      fix[0] = copy[3];
      fix[1] = copy[4];
      fix = order(fix, 5, 'd');
      for (int i = 0; i < 3; i++) {
        for (int j = 4; j > 0; j--) {
          fix[j] = fix[j - 1];
        }
      }
      fix[0] = copy[0];
      fix[1] = copy[1];
      fix[2] = copy[2];
    } else if (length == 3) {
      fix[0] = copy[2];
      fix[1] = copy[3];
      fix[2] = copy[4];
      fix = order(fix, 5, 'd');
      for (int i = 0; i < 2; i++) {
        for (int j = 4; j > 0; j--) {
          fix[j] = fix[j - 1];
        }
      }
      fix[0] = copy[0];
      fix[1] = copy[1];
    }
    copy = fix;
  }
  return copy;
}

struct card* evalpairs(struct player* current) {
  struct card* stuff = getcards(4);
  struct card* pairs = getcards(4);
  pairs = current->pairs;
  if (pairs[0].value != -1) {
    current->flags.op = pairs[0].value;
    if (pairs[1].value != pairs[0].value) {
      current->flags.tp = pairs[1].value;
      if (pairs[2].value == pairs[1].value) {
        current->flags.fh = pairs[2].value;
        current->flags.toak = pairs[2].value;
        if (pairs[3].value == pairs[2].value) {
          current->flags.foak = pairs[3].value;
        }
      } else if (pairs[2].value != pairs[1].value &&
                 pairs[2].value == pairs[3].value) {
        current->flags.fh = pairs[3].value;
        current->flags.toak = pairs[3].value;
      }
    } else if (pairs[0].value == pairs[1].value) {
      current->flags.toak = pairs[1].value;
      if (pairs[2].value == pairs[1].value) {
        current->flags.foak = pairs[2].value;
      } else if (pairs[2].value != pairs[1].value && pairs[2].value != -1) {
        current->flags.fh = pairs[0].value;
        current->flags.tp = pairs[2].value;
      }
    }
  } else {
    setflags2(current);
  }
  if (current->checked == -1) {
    stuff = fill(current);
  } else if (current->checked == 1) {
    stuff = getcards(5);
  }
  current->checked = 1;
  return stuff;
}

struct card* flush(struct player* current) {
  int C = 0, D = 0, H = 0, S = 0, index = 0;
  struct card* cards = getcards(7);
  cards = order(current->full, 7, 'a');
  for (int i = 0; i < 7; i++) {
    if (cards[i].type == 'C') {
      C++;
    } else if (cards[i].type == 'D') {
      D++;
    } else if (cards[i].type == 'H') {
      H++;
    } else {
      S++;
    }
  }
  if (C > 4) {
    current->flags.flush = 'C';
  } else if (D > 4) {
    current->flags.flush = 'D';
  } else if (H > 4) {
    current->flags.flush = 'H';
  } else if (S > 4) {
    current->flags.flush = 'S';
  }
  struct card* samesuit = getcards(7);
  for (int i = 0; i < 7; i++) {
    if (cards[i].type == current->flags.flush) {
      samesuit[index] = cards[i];
      index++;
    }
  }
  samesuit = order(samesuit, index, 'd');
  struct card* flush = getcards(5);
  flush = samesuit;
  current->flags.flush_num = flush[0].value;
  return flush;
}

struct card* straightflush(struct player* current) {
  struct card* straight = getcards(5);
  if (current->flags.flush != '\0' && current->flags.straight != -1) {
    struct card* cards = getcards(7);
    cards = order(current->full, 7, 'a');
    struct card* another = getcards(7);
    char suit = current->flags.flush;
    int index = 0;
    for (int i = 0; i < 7; i++) {
      if (cards[i].type == suit) {
        another[index] = cards[i];
        index++;
      }
    }
    int new_length = index, count = 1, max = 0;
    another = order(another, new_length, 'd');
    straight[0] = another[0];
    for (int i = 1; i < index; i++) {
      if (another[i].value != another[i - 1].value - 1 && count < 5) {
        count = 1;
        continue;
      } else if (count == 5) {
        break;
      } else if (another[i].value == another[i - 1].value - 1) {
        if (count == 1) {
          max = another[i - 1].value;
        }
        straight[count - 1] = another[i];
        count++;
      }
    }
    another = order(another, new_length, 'a');
    if (another[0].value == 2 && another[1].value == 3 &&
        another[2].value == 4 && another[3].value == 5 &&
        another[index - 1].value == 14) {
      current->flags.sf = 5;
    } else if (count >= 5) {
      current->flags.sf = max;
      if (max == 14) {
        current->flags.rf = 999;
      }
    }
    return another;
  }
  return straight;
}

void best(struct player* p) {
  struct card* cards = getcards(5);
  int rank = -100;
  if (p->flags.rf == 999) {
    rank += 10000000;
  } else if (p->flags.sf > 0) {
    rank += 2709000 * p->flags.sf;
    cards = straightflush(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 5);
  } else if (p->flags.foak > 0) {
    rank += 387000 * p->flags.foak;
    cards = evalpairs(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 4);
  } else if (p->flags.fh > 0) {
    rank += 5015 * ((10 * p->flags.toak) + p->flags.op);
    cards = evalpairs(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 4);
  } else if (p->flags.flush > 0) {
    rank += 7880 * p->flags.flush_num;
    cards = flush(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 5);
  } else if (p->flags.straight > 0) {
    rank += 1125 * p->flags.straight;
    cards = straight(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 5);
  } else if (p->flags.toak > 0) {
    rank += 400 * p->flags.toak;
    cards = evalpairs(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 4);
  } else if (p->flags.tp > 0 && p->flags.op > 0) {
    rank += 798;
    cards = evalpairs(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 4);
  } else if (p->flags.tp > 0) {
    rank += 57 * p->flags.tp;
    cards = evalpairs(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 4);
  } else if (p->flags.op > 0) {
    rank += 8 * p->flags.op;
    cards = evalpairs(p);
    memcpy(p->final, (void*)cards, sizeof(struct card) * 4);
  } else {
    struct card* nothing = getcards(7);
    nothing = order(p->full, 7, 'd');
    memcpy(p->final, (void*)nothing, sizeof(struct card) * 7);
  }
  evalpairs(p);
  p->rank = rank;
  return;
}

struct player* greatest(struct player* p1, struct player* p2) {
  struct card* draw1 = getcards(5);
  struct card* draw2 = getcards(5);
  draw1 = p1->final;
  draw2 = p2->final;
  for (int i = 0; i < 5; i++) {
    if (draw1[i].value > draw2[i].value) {
      return p1;
      break;
    } else if (draw1[i].value < draw2[i].value) {
      return p2;
    }
  }
  return p1;
}

char* printval(int val) {
  if (val == 2) {
    return "Two";
  } else if (val == 3) {
    return "Three";
  } else if (val == 4) {
    return "Four";
  } else if (val == 5) {
    return "Five";
  } else if (val == 6) {
    return "Six";
  } else if (val == 7) {
    return "Seven";
  } else if (val == 8) {
    return "Eight";
  } else if (val == 9) {
    return "Nine";
  } else if (val == 10) {
    return "Ten";
  } else if (val == 11) {
    return "Jack";
  } else if (val == 12) {
    return "Queen";
  } else if (val == 13) {
    return "King";
  } else if (val == 14) {
    return "Ace";
  }
  return "";
}

void printcards(struct player current, int flag) {
  if (flag == 2) {
    printf(" %s", printval(current.final[2].value));
  } else if (flag == 3) {
    for (int i = 0; i < 4; i++) {
      printf(" %s", printval(current.final[i].value));
    }
  } else if (flag == 4) {
    for (int i = 1; i < 4; i++) {
      printf(" %s", printval(current.final[i].value));
    }
  } else if (flag == 5) {
    printf("%d", current.flags.foak);
    printf(" %s", printval(current.final[0].value));
  } else {
    for (int i = 0; i < 5; i++) {
      if (current.final[i].value != -1) {
        printf(" %s", printval(current.final[i].value));
      }
    }
  }
}

void verbose() {
  for (int i = 0; i < 3; i++) {
    printf("\nP%d:", i + 1);
    if (p[i].flags.rf == 999) {
      printf(" Straight Flush");
      printcards(p[i], -1);
    } else if (p[i].flags.sf > 0) {
      printf(" Straight Flush ");
      printcards(p[i], -1);
    } else if (p[i].flags.foak > 0) {
      printf(" Quad ");
      printcards(p[i], 5);
    } else if (p[i].flags.fh > 0) {
      printf(" Full House. Three-of-a-kind. %s Pair of %s",
             printval(p[i].flags.toak), printval(p[i].flags.op));
    } else if (p[i].flags.flush > 0) {
      printf(" Flush");
      printcards(p[i], -1);
    } else if (p[i].flags.straight > 0) {
      printf(" Straight");
      printcards(p[i], 1);
    } else if (p[i].flags.toak > 0) {
      printf(" Three-of-a-kind");
      printcards(p[i], 4);
    } else if (p[i].flags.tp > 0 && p[i].flags.op > 0) {
      printf(" Pairs of ");
      printf("%s", printval(p[i].flags.op));
      printf(" Pairs of ");
      printf("%s", printval(p[i].flags.tp));
      printcards(p[i], 2);
    } else if (p[i].flags.tp > 0) {
      printf("Two Pair ");
      printcards(p[i], -1);
    } else if (p[i].flags.op > 0) {
      printf(" Pair of");
      printcards(p[i], 3);
    } else {
      printcards(p[i], -1);
    }
  }
}

int main(int argc, const char* argv[]) {
  const char* filename = (char*)malloc(sizeof(argv[1]));
  filename = argv[1];
  FILE* buffer = fopen(filename, "r");
  char line[256];
  int nol = 1;
  while (fgets(line, sizeof(line), buffer)) {
    assign(line);
    for (int i = 0; i < 3; i++) {
      quantify(&p[i].hand[0]);
      quantify(&p[i].hand[1]);
    }
    for (int i = 0; i < 5; i++) {
      quantify(&t.community[i]);
    }
    for (int i = 0; i < 3; i++) {
      qna(&p[i], &t, i);
      setflags(&p[i], i);
      findpairs(&p[i]);
      evalpairs(&p[i]);
      qna(&p[i], &t, i);
      flush(&p[i]);
      straight(&p[i]);
      straightflush(&p[i]);
      best(&p[i]);
    }
    FILE* buffer1 = fopen("output.txt", "a");
    struct player* players = (struct player*)malloc(sizeof(struct player) * 3);
    players[0] = p[0];
    players[1] = p[1];
    players[2] = p[2];
    for (int i = 0; i < 3; i++) {
      for (int j = i + 1; j < 3; j++) {
        if (players[i].rank > players[j].rank) {
          struct player temp = players[i];
          players[i] = players[j];
          players[j] = temp;
        }
      }
    }
    printf("\n*********** Hand Number: %d ***********", nol);
    if (players[0].rank == players[1].rank &&
        players[1].rank == players[2].rank) {
      struct player* best = (struct player*)malloc(sizeof(struct player));
      best = greatest(&players[0], &players[1]);
      best = greatest(best, &players[2]);
      printf("\nPlayer %d wins.", best->id);
      fprintf(buffer1, "Player %d wins.\n", best->id);
    } else if (players[1].rank == players[2].rank) {
      struct player* best = (struct player*)malloc(sizeof(struct player));
      best = greatest(&players[1], &players[2]);
      printf("\nPlayer %d wins.", best->id);
      fprintf(buffer1, "Player %d wins.\n", best->id);
    } else {
      printf("\nPlayer %d wins.", players[2].id);
      fprintf(buffer1, "Player %d wins.\n", players[2].id);
    }
    verbose();
    fclose(buffer1);
    nol++;
  }
  fclose(buffer);
  return 0;
}