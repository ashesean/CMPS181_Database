// By Sean Ashe
// Query Engine


#include "qe.h"


// Helper functions

bool Helper::checkCondition(vector<Attribute> attrs, const void *data, const Condition &condition)
{
    if (attrs.size() == 0)
        return false;
    
    void *lhsData = malloc(PAGE_SIZE);
    void *rhsData = malloc(PAGE_SIZE);
    bool lhsFound = false;
    bool rhsFound = false;
    AttrType lhsType, rhsType;
    unsigned offset = 0;
    unsigned stringLength;
    
    // Set the value of rhs if it is already provided
    if (condition.bRhsIsAttr == false)
    {
        memcpy(rhsData, condition.rhsValue.data, PAGE_SIZE);
        rhsType = condition.rhsValue.type;
        rhsFound = true;
    }
    
    // Go through the attributes and assign appropriate values to lhs and rhs
    for (unsigned i = 0; i < attrs.size() && (!lhsFound || !rhsFound); i++)
    {
        if (attrs[i].type == TypeInt)
        {
            if (!lhsFound && (attrs[i].name).compare(condition.lhsAttr) == 0)
            {
                memcpy(lhsData, (char*) data + offset, INT_SIZE);
                lhsType = attrs[i].type;
                lhsFound = true;
            }
            if (!rhsFound && (attrs[i].name).compare(condition.rhsAttr) == 0)
            {
                memcpy(rhsData, (char*) data + offset, INT_SIZE);
                rhsType = attrs[i].type;
                rhsFound = true;
            }
            offset += INT_SIZE;
        }
        else if (attrs[i].type == TypeReal)
        {
            if (!lhsFound && (attrs[i].name).compare(condition.lhsAttr) == 0)
            {
                memcpy(lhsData, (char*) data + offset, REAL_SIZE);
                lhsType = attrs[i].type;
                lhsFound = true;
            }
            if (!rhsFound && (attrs[i].name).compare(condition.rhsAttr) == 0)
            {
                memcpy(rhsData, (char*) data + offset, REAL_SIZE);
                rhsType = attrs[i].type;
                rhsFound = true;
            }
            offset += REAL_SIZE;
        }
        else
        {
            memcpy(&stringLength, (char*) data + offset, VARCHAR_LENGTH_SIZE);
            if (!lhsFound && (attrs[i].name).compare(condition.lhsAttr) == 0)
            {
                memcpy(lhsData, (char*) data + offset, stringLength + VARCHAR_LENGTH_SIZE);
                lhsType = attrs[i].type;
                lhsFound = true;
            }
            if (!rhsFound && (attrs[i].name).compare(condition.rhsAttr) == 0)
            {
                memcpy(rhsData, (char*) data + offset, stringLength + VARCHAR_LENGTH_SIZE);
                rhsType = attrs[i].type;
                rhsFound = true;
            }
            offset += stringLength + VARCHAR_LENGTH_SIZE;
        }
    }
    
    // if there was no match for lhs or rhs, return false
    if (!lhsFound || !rhsFound)
        return false;
    
    // if lhs and rhs aren't compatible, return false
    if (lhsType != rhsType)
        return false;
    
    // compare lhs and rhs
    bool result;
    if (lhsType == TypeInt)
        result = compareInts(lhsData, rhsData, condition.op);
    else if (lhsType == TypeReal)
        result = compareFloats(lhsData, rhsData, condition.op);
    else
        result = compareStrings(lhsData, rhsData, condition.op);
    
    free(lhsData);
    free(rhsData);
    return result;
}

bool Helper::compareInts(const void *lhs, const void *rhs, CompOp op)
{
    int leftValue, rightValue;
    memcpy(&leftValue, lhs, INT_SIZE);
    memcpy(&rightValue, rhs, INT_SIZE);
    
    switch (op)
    {
        case EQ_OP:  // =
            return leftValue == rightValue;
            break;
        case LT_OP:  // <
            return leftValue < rightValue;
            break;
        case GT_OP:  // >
            return leftValue > rightValue;
            break;
        case LE_OP:  // <=
            return leftValue <= rightValue;
            break;
        case GE_OP:  // >=
            return leftValue >= rightValue;
            break;
        case NE_OP:  // !=
            return leftValue != rightValue;
            break;
        case NO_OP:
            return true;
            break;
    }
}

bool Helper::compareFloats(const void *lhs, const void *rhs, CompOp op)
{
    float leftValue, rightValue;
    memcpy(&leftValue, lhs, REAL_SIZE);
    memcpy(&rightValue, rhs, REAL_SIZE);
    
    switch (op)
    {
        case EQ_OP:  // =
            return leftValue == rightValue;
            break;
        case LT_OP:  // <
            return leftValue < rightValue;
            break;
        case GT_OP:  // >
            return leftValue > rightValue;
            break;
        case LE_OP:  // <=
            return leftValue <= rightValue;
            break;
        case GE_OP:  // >=
            return leftValue >= rightValue;
            break;
        case NE_OP:  // !=
            return leftValue != rightValue;
            break;
        case NO_OP:
            return true;
            break;
    }
}

bool Helper::compareStrings(const void *lhs, const void *rhs, CompOp op)
{
    unsigned leftStringLength, rightStringLength;
    bool result;
    
    // Get the string lengths
    memcpy(&leftStringLength, (char*) lhs, VARCHAR_LENGTH_SIZE);
    memcpy(&rightStringLength, (char*) rhs, VARCHAR_LENGTH_SIZE);
    
    // Copies the strings data.
    char *leftValue = (char*) malloc(leftStringLength + 1);
    char *rightValue = (char*) malloc(rightStringLength + 1);
    memcpy(leftValue, (char*) lhs + VARCHAR_LENGTH_SIZE, leftStringLength);
    memcpy(rightValue, (char*) rhs + VARCHAR_LENGTH_SIZE, rightStringLength);
    
    // Adds the string terminators.
    leftValue[leftStringLength] = '\0';
    rightValue[rightStringLength] = '\0';
    
    switch (op)
    {
        case EQ_OP:  // =
            result = strcmp(leftValue, rightValue) == 0;
            break;
        case LT_OP:  // <
            result = strcmp(leftValue, rightValue) < 0;
            break;
        case GT_OP:  // >
            result = strcmp(leftValue, rightValue) > 0;
            break;
        case LE_OP:  // <=
            result = strcmp(leftValue, rightValue) <= 0;
            break;
        case GE_OP:  // >=
            result = strcmp(leftValue, rightValue) >= 0;
            break;
        case NE_OP:  // !=
            result = strcmp(leftValue, rightValue) != 0;
            break;
        case NO_OP:
            result = true;
            break;
    }
    
    free(leftValue);
    free(rightValue);
    return result;
}

unsigned Helper::getRecordSize(const vector<Attribute> &recordDescriptor, const void *data)
{
    unsigned size = 0;
	unsigned varcharSize = 0;
    
	for (unsigned i = 0; i < (unsigned) recordDescriptor.size(); i++)
		switch (recordDescriptor[i].type)
    {
        case TypeInt:
            size += INT_SIZE;
			break;
        case TypeReal:
            size += REAL_SIZE;
			break;
        case TypeVarChar:
            // We have to get the size of the VarChar field by reading the integer that precedes the string value itself.
            memcpy(&varcharSize, (char*) data + size, VARCHAR_LENGTH_SIZE);
            // We also have to account for the overhead given by that integer.
            size += VARCHAR_LENGTH_SIZE + varcharSize;
			break;
    }
    
	return size;
}


// Filter functions

Filter::Filter(Iterator* input, const Condition &condition)
{
    filterIterator = input;
    filterCondition = condition;
    getAttributes(filterAttributes);
}

RC Filter::getNextTuple(void *data)
{
    if (filterIterator->getNextTuple(data) == QE_EOF)
        return QE_EOF;
    
    Helper helper;
    if (!helper.checkCondition(filterAttributes, data, filterCondition))
        return getNextTuple(data);
    
    return SUCCESS;
}

void Filter::getAttributes(vector<Attribute> &attrs) const
{
    filterIterator->getAttributes(attrs);
}


// Project functions

Project::Project(Iterator *input, const vector<string> &attrNames)
{
    projectIterator = input;
    projectAttrNames = attrNames;
}

RC Project::getNextTuple(void *data)
{
    void *tuple = malloc(PAGE_SIZE);
    
    if (projectIterator->getNextTuple(tuple) == QE_EOF)
        return QE_EOF;
    
    vector<Attribute> iteratorAttrs;
    vector<Attribute> attrs;
    projectIterator->getAttributes(iteratorAttrs);
    getAttributes(attrs);
    
    unsigned tupleOffset, dataOffset = 0;
    unsigned i, j = 0;
    unsigned length, stringLength;
    
    while (i < iteratorAttrs.size() && j < attrs.size())
    {
        if (iteratorAttrs[i].type == TypeInt)
            length = INT_SIZE;
        else if (iteratorAttrs[i].type == TypeReal)
            length = REAL_SIZE;
        else
        {
            memcpy(&stringLength, (char*) tuple + tupleOffset, VARCHAR_LENGTH_SIZE);
            length = VARCHAR_LENGTH_SIZE + stringLength;
        }
        
        if ((iteratorAttrs[i].name).compare(attrs[j].name) == 0)
        {
            memcpy((char*) data + dataOffset, (char*) tuple + tupleOffset, length);
            dataOffset += length;
            j++;
        }
        
        tupleOffset += length;
        i++;
    }
    
    return SUCCESS;
}

void Project::getAttributes(vector<Attribute> &attrs) const
{
    vector<Attribute> iteratorAttrs;
    projectIterator->getAttributes(iteratorAttrs);
    
    attrs.clear();
    bool found;
    
    // Populate attrs with the desired Attributes
    for (unsigned i = 0; i < projectAttrNames.size(); i++)
    {
        found = false;
        for (unsigned j = 0; j < iteratorAttrs.size() && !found; j++)
        {
            if (projectAttrNames[i].compare(iteratorAttrs[j].name) == 0)
            {
                attrs.push_back(iteratorAttrs[j]);
                found = true;
            }
        }
    }
}


// NL Join functions

NLJoin::NLJoin(Iterator *leftIn, TableScan *rightIn, const Condition &condition, const unsigned numPages)
{
    left = leftIn;
    right = rightIn;
    joinCondition = condition;
    this->numPages = numPages;
    goLeft = true;
    
    leftTuple = malloc(PAGE_SIZE);
    rightTuple = malloc(PAGE_SIZE);
    
    left->getAttributes(leftAttrs);
    right->getAttributes(rightAttrs);
    getAttributes(joinAttrs);
    
    right->setIterator();
}

RC NLJoin::getNextTuple(void *data)
{
    // Get next left tuple
    if (goLeft)
    {
        if (left->getNextTuple(leftTuple) == QE_EOF)
            return QE_EOF;
        goLeft = false;
    }
    
    // Get next right tuple
    if (right->getNextTuple(rightTuple) == QE_EOF)
    {
        right->setIterator();
        goLeft = true;
        return getNextTuple(data);
    }
    
    // Set up the condition
    Condition condition;
    if (joinCondition.bRhsIsAttr)
    {
        condition.lhsAttr = joinCondition.lhsAttr;
        condition.op = joinCondition.op;
        condition.bRhsIsAttr = false;
        condition.rhsAttr = joinCondition.rhsAttr;
        bool found = false;
        unsigned offset = 0;
        Value value;
        value.data = malloc(PAGE_SIZE);
        
        for (unsigned i = 0; i < rightAttrs.size() && !found; i++)
        {
            if (rightAttrs[i].type == TypeInt)
            {
                if (rightAttrs[i].name.compare(condition.rhsAttr) == 0)
                {
                    memcpy(value.data, (char*) rightTuple + offset, INT_SIZE);
                    value.type = rightAttrs[i].type;
                    found = true;
                }
                offset += INT_SIZE;
            }
            else if (rightAttrs[i].type == TypeReal)
            {
                if (rightAttrs[i].name.compare(condition.rhsAttr) == 0)
                {
                    memcpy(value.data, (char*) rightTuple + offset, REAL_SIZE);
                    value.type = rightAttrs[i].type;
                    found = true;
                }
                offset += REAL_SIZE;
            }
            else
            {
                unsigned stringLength;
                memcpy(&stringLength, (char*) rightTuple + offset, VARCHAR_LENGTH_SIZE);
                if (rightAttrs[i].name.compare(condition.rhsAttr) == 0)
                {
                    memcpy(value.data, (char*) rightTuple + offset, stringLength + VARCHAR_LENGTH_SIZE);
                    value.type = rightAttrs[i].type;
                    found = true;
                }
                offset += stringLength + VARCHAR_LENGTH_SIZE;
            }
        }
        
        condition.rhsValue = value;
        free(value.data);
    }
    else
        condition = joinCondition;
    
    // Check the condition
    Helper helper;
    if (!helper.checkCondition(leftAttrs, leftTuple, condition))
        return getNextTuple(data);
    
    // Get here when checkCondition is true
    unsigned leftSize = helper.getRecordSize(leftAttrs, leftTuple);
    unsigned rightSize = helper.getRecordSize(rightAttrs, rightTuple);
    
    memcpy(data, leftTuple, leftSize);
    memcpy((char*) data + leftSize, rightTuple, rightSize);
    
    return SUCCESS;
}

void NLJoin::getAttributes(vector<Attribute> &attrs) const
{
    attrs.clear();
    
    // Do left side
    for (int i = 0; i < leftAttrs.size(); i++)
        attrs.push_back(leftAttrs[i]);
    
    // Do right side
    for (int i = 0; i < rightAttrs.size(); i++)
        attrs.push_back(rightAttrs[i]);
}





