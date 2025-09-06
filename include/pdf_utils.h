// include/pdf_utils.h
#pragma once
#include <string>
bool validatePdfPassword(const std::string& pdfPath, const std::string& password); // dummy for compatibility
std::string extractPdfHash(const std::string& pdfPath);
int detectPdfHashMode(const std::string& hash);
