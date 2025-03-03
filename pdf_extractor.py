#!/usr/bin/env python3

import sys
import os
import subprocess
import importlib.util

# Check if PyPDF2 is installed, if not install it
def check_install_pypdf2():
    if importlib.util.find_spec("PyPDF2") is None:
        print("PyPDF2 is not installed. Installing now...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "PyPDF2"])
        print("PyPDF2 installed successfully.")
    else:
        print("PyPDF2 is already installed.")

def extract_text_from_pdf(pdf_path):
    """Extract text from a PDF file and return it as a string."""
    try:
        from PyPDF2 import PdfReader
        print(f"Opening PDF: {pdf_path}")
        
        if not os.path.exists(pdf_path):
            print(f"Error: File not found at {pdf_path}")
            return None
        
        reader = PdfReader(pdf_path)
        text = ""
        
        print(f"PDF has {len(reader.pages)} pages.")
        
        for i, page in enumerate(reader.pages):
            print(f"Extracting page {i+1}...")
            page_text = page.extract_text()
            text += page_text + "\n\n"
        
        return text
    except Exception as e:
        print(f"Error extracting text from PDF: {e}")
        return None

def save_to_file(text, output_path):
    """Save the extracted text to a file."""
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(text)
        print(f"Text saved to {output_path}")
    except Exception as e:
        print(f"Error saving text to file: {e}")

def main():
    # PDF file path
    pdf_path = "assignment_2.pdf"
    output_path = "assignment_2_content.txt"
    
    # Ensure PyPDF2 is installed
    check_install_pypdf2()
    
    # Extract text
    extracted_text = extract_text_from_pdf(pdf_path)
    
    if extracted_text:
        print("\nExtracted text:")
        print("-" * 40)
        print(extracted_text[:500] + "..." if len(extracted_text) > 500 else extracted_text)
        print("-" * 40)
        
        # Save to file
        save_to_file(extracted_text, output_path)
        print(f"Full text has been saved to {output_path}")

if __name__ == "__main__":
    main()