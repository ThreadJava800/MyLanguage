#ifndef SYNTAX_H
#define SYNTAX_H

const char baseOpers[] = {'+', '-', '*', '/', '^', '=', '>', '<', '|', '&', ';', '(', ')', '{', '}'};

const char* const ifCom    = "if";
const char* const whileCom = "while";
const char* const elseCom  = "else";
const char* const varCom   = "var";
const char* const assCom   = "now";
const char* const outCom   = "getout";
const char* const inCom    = "gimme";
const char* const equCom   = "look";
const char* const defCom   = "hh";
const char* const retCom   = "to_production";
const char* const callCom  = "runmf";
const char* const sqrtCom  = "koreshok";
const char* const cosCom   = "cos";

const char* const allowedVarNames[] = {
    "LINUX",
    "C",
    "ALKORYTHM",
    "HLOP_HLOP",
    "DEMIDOVICH",
    "JAVA",
    "DEADLINE",
    "POLYMORPHYSM",
    "SOMA_COLA",
    "PO",
    "ARIA",
};

#endif