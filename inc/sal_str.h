#ifndef SAL_STR_H_
#define SAL_STR_H_

#include "sal.h"

typedef struct{
    char *begin;
    uint32_t size;
    bool dalloced;
} sal_str;

extern const sal_str SAL_NULL_STR;

bool sal_str_isempty(sal_str str);
bool sal_str_isnull(sal_str str);
bool sal_str_isequal(sal_str s1, sal_str s2);
void sal_println(sal_str str);
sal_str sal_str_init(char *cstr);
void sal_str_deinit(sal_str str);
sal_str sal_str_cat(sal_str to, sal_str from);
sal_str sal_str_add(sal_str to, sal_str from);
sal_str sal_tostr(char *cstr);
void sal_str_reset_chop(void);
sal_str sal_str_save_chop();
void sal_str_load_chop(sal_str str);
sal_str sal_str_chop(sal_str target, sal_str delim);
sal_str sal_str_isinclude(sal_str str, sal_str inc);
sal_str sal_str_tolower(sal_str str);
sal_str sal_str_toupper(sal_str str);
void sal_str_cpytobuffer(char *target, sal_str str);
void sal_str_ncpytobuffer(char *target, sal_str str, uint32_t size);
int32_t sal_str_toint(sal_str str);
sal_str sal_str_cpy(sal_str to, sal_str from);
sal_str sal_str_ncpy(sal_str to, sal_str from, uint32_t size);

#ifdef SAL_IMPLEMENTATION
const sal_str SAL_NULL_STR = {0};

sal_str sal_tostr(char *cstr){
    sal_str res = {0};
    if(cstr == NULL) return res;
    res.begin = cstr;
    res.size = strlen(cstr);
    return res;
}

sal_str sal_str_init(char *cstr) {
    sal_str res = { .dalloced = true };
    res.begin = sal_darray_new(sizeof(char));

    if(cstr) {
	res.size = strlen(cstr);
	sal_darray_copyall((void**)&res.begin, cstr, res.size);
    }
    return res;
}

void sal_str_deinit(sal_str str) {
    if(!str.dalloced) return;
    sal_darray_free(str.begin);
}

sal_str sal_str_cat(sal_str dest, sal_str src) {
  if(!dest.dalloced) return SAL_NULL_STR;
    sal_darray_copyall((void**)&dest.begin, src.begin, src.size);
    dest.size += src.size;
    return dest;
}

sal_str sal_str_add(sal_str str, sal_str add) {
    memcpy(str.begin + str.size, add.begin, add.size);
    str.size += add.size;
    return str;
}

void sal_print(sal_str str){
    // add internal buffer for speed
    if(str.begin == NULL) return;
    for(uint32_t count = 0; count < str.size; ++count){
	putc(str.begin[count], stdout);
    }
}

void sal_println(sal_str str){
    sal_print(str);
    putc('\n', stdout);
}

bool sal_str_isequal(sal_str s1, sal_str s2) {
    if(s1.begin == NULL && s2.begin == NULL) return true;
    if(s1.begin == NULL || s2.begin == NULL || s1.size != s2.size) return false;
    
    bool res = true;
    for(uint32_t count = 0 ; s1.size > 0 && s2.size > 0 ; --s1.size, --s2.size, ++count){
	if(s1.begin[count] != s2.begin[count]){
	    res = false;
	    break;
	}
    }
    return res;
}

bool sal_str_isempty(sal_str str) {
    return !(str.size);
}

bool sal_str_isnull(sal_str str) { return sal_str_isequal(str, SAL_NULL_STR); }

sal_str sal_str_tolower(sal_str str) {
    for(uint32_t i = 0; i < str.size ; ++i) {
	if(str.begin[i] >= 65 && str.begin[i] <= 90)
	    str.begin[i] += 32;
    }

    return str;
}

sal_str sal_str_toupper(sal_str str) {
    for(uint32_t i = 0; i < str.size; ++i){
	if(str.begin[i] >= 97 && str.begin[i] <= 122)
	    str.begin[i] -= 32;
    }

    return str;
}

static sal_str cont_target = {0};
void sal_str_reset_chop(void) { cont_target = SAL_NULL_STR; }
sal_str sal_str_save_chop() { return cont_target; }
void sal_str_load_chop(sal_str str) { cont_target = str; }

sal_str sal_str_isinclude(sal_str str, sal_str inc) {
    if(sal_str_isempty(inc)) return SAL_NULL_STR;
    sal_str tmp = {
	.size = inc.size,
	.begin = str.begin
    };

    while(tmp.begin < str.begin + str.size - inc.size) {
	if(sal_str_isequal(tmp, inc))
	    return tmp;
	++tmp.begin;
    }

    return SAL_NULL_STR;
}


sal_str sal_str_chop(sal_str target, sal_str delim) {
    if(sal_str_isempty(delim)) return SAL_NULL_STR;
    
    bool cont = sal_str_isempty(target);
    sal_str res = SAL_NULL_STR;
    sal_str found_delim = {
	.size = delim.size
    };

    if(!cont) cont_target = target;
    found_delim.begin = cont_target.begin;

    bool found = false;
    while (found_delim.begin < (cont_target.begin + cont_target.size - delim.size)) {
	if(sal_str_isequal(found_delim, delim)) {
	    found = true;
	    break;
	}
	else ++found_delim.begin;
    }

    if(found) {
	res.begin = cont_target.begin;
	res.size = found_delim.begin - cont_target.begin;
	cont_target.begin = found_delim.begin + delim.size;
	cont_target.size -= (res.size + delim.size);
    }
    else if(found_delim.begin + found_delim.size == cont_target.begin + cont_target.size &&
	    cont) { // last token without delim
	res.begin = cont_target.begin;
	res.size = cont_target.size;
	cont_target.begin += cont_target.size;
	cont_target.size = 0;
    }
    
    return res;
}

void sal_str_cpytobuffer(char *target, sal_str str) {
    if(target == NULL) return;
    for(uint32_t i = 0; i < str.size; ++i) {
	target[i] = str.begin[i];
    }
    target[str.size] = 0;
}

void sal_str_ncpytobuffer(char *target, sal_str str, uint32_t size) {
    if(target == NULL) return;
    uint32_t up_bound = str.size < size ? str.size : size;
    for(uint32_t i = 0; i < up_bound; ++i) {
	target[i] = str.begin[i];
    }
    target[str.size] = 0;
}

int32_t sal_str_toint(sal_str str) {
    char buff[16] = {0};
    sal_str_cpytobuffer(buff, str);
    return atoi(buff);
}

sal_str sal_str_cpy(sal_str to, sal_str from) {
    for (uint32_t i = 0; i < from.size; ++i) {
	to.begin[i] = from.begin[i];
    }
    to.size = from.size;
    return to;
}

sal_str sal_str_ncpy(sal_str to, sal_str from, uint32_t size) {
    uint32_t up_bound = from.size < size ? from.size : size;
    for (uint32_t i = 0; i < up_bound; ++i) {
	to.begin[i] = from.begin[i];
    }
    to.size = from.size;
    return to;
}

#endif // SAL_IMPLEMENTATION
#endif // SAL_STR_H_
