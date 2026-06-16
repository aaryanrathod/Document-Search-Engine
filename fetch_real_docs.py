import os
from sklearn.datasets import fetch_20newsgroups

# Create a directory for our real-world dataset
os.makedirs("real_text_docs", exist_ok=True)

print("Downloading the 20 Newsgroups dataset (this might take a minute)...")
# We remove headers and footers to just get the raw, natural text body
newsgroups_data = fetch_20newsgroups(subset='all', remove=('headers', 'footers', 'quotes'))

print(f"Successfully downloaded {len(newsgroups_data.data)} real documents. Saving to text files...")

valid_doc_count = 0

for i, text in enumerate(newsgroups_data.data):
    # Skip documents that are completely empty after stripping
    if not text.strip():
        continue
    
    # Save each document as a separate text file
    with open(f"real_text_docs/doc_{valid_doc_count}.txt", "w", encoding="utf-8") as f:
        f.write(text)
    
    valid_doc_count += 1

print(f"Done! You now have {valid_doc_count} highly realistic text files to benchmark your search engine.")