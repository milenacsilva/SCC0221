#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <baseFunctions.h>

#define FIELD_INFO_INDEX 1

typedef enum {
    STRING,
    INTEGER,
    FLOAT,
    DOUBLE,
} ValueTypes;

typedef struct {
    char *name;
    int type;
    int size;
} Field;

typedef struct {
    FILE *sourceFile;
    Field *fields;
    int offsetSize;
    int amountOfFields;
} Metadata;

void initField(Metadata *metadata);
Metadata* initMetadata(FILE *metadataFile);
Metadata* getMetadata(FILE *metadataFile);
void freeMetadata(Metadata *metadata);
int getFieldType(Metadata *metadata, int fieldIndex, char **tokens);
