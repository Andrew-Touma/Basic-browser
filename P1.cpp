#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <cstring>
using namespace std;

struct related_Pages {
    int R_ID;
    string URL;
    string title;
    related_Pages* next;
};

struct Page {
    int P_ID;
    string URL;
    string title;
    time_t visitedAt;
    Page* prev, * next;
    related_Pages* rP_head;
};

struct pages_History {
    Page* head, * tail;
};

struct bookMarkNode {
    Page* page;
    bool favorite;
    int visitCount;
    time_t lastVisited;
    bookMarkNode* prev, * next;
};

struct bookMarkList {
    bookMarkNode* head, * tail;
};

time_t parseDate(const string& dateStr) {
    struct tm tm = { 0 };
    int day, month, year, hour, min;
    sscanf_s(dateStr.c_str(), "%d/%d/%d %d:%d", &day, &month, &year, &hour, &min);
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_hour = hour;
    tm.tm_min = min;
    return mktime(&tm);
}

string formatDate(time_t t) {
    struct tm tm;
    localtime_s(&tm, &t);
    char buffer[20];
    sprintf_s(buffer, 20, "%02d/%02d/%d %02d:%02d", tm.tm_mday, tm.tm_mon + 1,
        tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
    return string(buffer);
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void parseInputFile(const string& filename, pages_History& history, bookMarkList& bookmarks) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    history.head = history.tail = nullptr;
    bookmarks.head = bookmarks.tail = nullptr;

    string line;
    Page* currentPage = nullptr;

    while (getline(file, line)) {
        if (line.empty()) continue;

        if (line[0] == '-') {

            Page* newPage = new Page();
            newPage->prev = newPage->next = nullptr;
            newPage->rP_head = nullptr;

            size_t pos = 1;
            size_t comma1 = line.find(',', pos);
            newPage->P_ID = stoi(trim(line.substr(pos, comma1 - pos)));

            pos = comma1 + 1;
            size_t comma2 = line.find(',', pos);
            newPage->URL = trim(line.substr(pos, comma2 - pos));

            pos = comma2 + 1;
            size_t comma3 = line.find(',', pos);
            newPage->title = trim(line.substr(pos, comma3 - pos));

            pos = comma3 + 1;
            newPage->visitedAt = parseDate(trim(line.substr(pos)));

            if (history.head == nullptr) {
                history.head = history.tail = newPage;
            }
            else {
                history.tail->next = newPage;
                newPage->prev = history.tail;
                history.tail = newPage;
            }

            currentPage = newPage;
        }
        else if (line[0] == '*' && currentPage != nullptr) {
            related_Pages* rp = new related_Pages();
            rp->next = nullptr;

            size_t pos = 1;
            size_t comma1 = line.find(',', pos);
            rp->R_ID = stoi(trim(line.substr(pos, comma1 - pos)));

            pos = comma1 + 1;
            size_t comma2 = line.find(',', pos);
            rp->URL = trim(line.substr(pos, comma2 - pos));

            pos = comma2 + 1;
            rp->title = trim(line.substr(pos));


            if (currentPage->rP_head == nullptr) {
                currentPage->rP_head = rp;
            }
            else {
                related_Pages* temp = currentPage->rP_head;
                while (temp->next != nullptr) {
                    temp = temp->next;
                }
                temp->next = rp;
            }
        }
        else if (line[0] == '#' && currentPage != nullptr) {

            bookMarkNode* bm = new bookMarkNode();
            bm->page = currentPage;
            bm->prev = bm->next = nullptr;

            size_t pos = 1;
            size_t comma1 = line.find(',', pos);
            string fav = trim(line.substr(pos, comma1 - pos));
            bm->favorite = (fav == "yes");

            pos = comma1 + 1;
            size_t comma2 = line.find(',', pos);
            bm->visitCount = stoi(trim(line.substr(pos, comma2 - pos)));

            pos = comma2 + 1;
            bm->lastVisited = parseDate(trim(line.substr(pos)));

            if (bookmarks.head == nullptr) {
                bookmarks.head = bookmarks.tail = bm;
            }
            else {
                bookmarks.tail->next = bm;
                bm->prev = bookmarks.tail;
                bookmarks.tail = bm;
            }
        }
    }

    file.close();
    cout << "File parsed successfully!" << endl;
}

void createPage(pages_History& history, int pageID, string url, string title,
    time_t visitTime, int relatedIDs[], string relatedURLs[],
    string relatedTitles[], int numRelated) {


    Page* newPage = new Page();
    newPage->P_ID = pageID;
    newPage->URL = url;
    newPage->title = title;
    newPage->visitedAt = visitTime;
    newPage->prev = newPage->next = nullptr;
    newPage->rP_head = nullptr;

    for (int i = 0; i < numRelated; i++) {
        related_Pages* rp = new related_Pages();
        rp->R_ID = relatedIDs[i];
        rp->URL = relatedURLs[i];
        rp->title = relatedTitles[i];
        rp->next = nullptr;

        if (newPage->rP_head == nullptr) {
            newPage->rP_head = rp;
        }
        else {
            related_Pages* temp = newPage->rP_head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = rp;
        }
    }

    if (history.head == nullptr) {
        history.head = history.tail = newPage;
    }
    else {
        history.tail->next = newPage;
        newPage->prev = history.tail;
        history.tail = newPage;
    }

    cout << "Page created successfully!" << endl;
}


void removeOldPages(pages_History& history, bookMarkList& bookmarks, time_t cutoffDate) {
    Page* current = history.head;
    int removedCount = 0;

    while (current != nullptr) {
        Page* next = current->next;

        if (current->visitedAt < cutoffDate) {

            bookMarkNode* bm = bookmarks.head;
            while (bm != nullptr) {
                bookMarkNode* nextBm = bm->next;
                if (bm->page == current) {

                    if (bm->prev) bm->prev->next = bm->next;
                    else bookmarks.head = bm->next;

                    if (bm->next) bm->next->prev = bm->prev;
                    else bookmarks.tail = bm->prev;

                    delete bm;
                }
                bm = nextBm;
            }

            if (current->prev) current->prev->next = current->next;
            else history.head = current->next;

            if (current->next) current->next->prev = current->prev;
            else history.tail = current->prev;

            related_Pages* rp = current->rP_head;
            while (rp != nullptr) {
                related_Pages* nextRp = rp->next;
                delete rp;
                rp = nextRp;
            }

            delete current;
            removedCount++;
        }

        current = next;
    }

    cout << "Removed " << removedCount << " page(s) older than " << formatDate(cutoffDate) << endl;
}

Page* findMostRelatedPage(pages_History& history) {
    Page* mostRelated = nullptr;
    int maxCount = 0;

    Page* current = history.head;
    while (current != nullptr) {
        int count = 0;
        related_Pages* rp = current->rP_head;
        while (rp != nullptr) {
            count++;
            rp = rp->next;
        }

        if (count > maxCount) {
            maxCount = count;
            mostRelated = current;
        }

        current = current->next;
    }

    return mostRelated;
}


void deletePage(pages_History& history, bookMarkList& bookmarks, int pageID) {
    Page* current = history.head;

    while (current != nullptr) {
        if (current->P_ID == pageID) {

            bookMarkNode* bm = bookmarks.head;
            while (bm != nullptr) {
                bookMarkNode* nextBm = bm->next;
                if (bm->page == current) {
                    if (bm->prev) bm->prev->next = bm->next;
                    else bookmarks.head = bm->next;

                    if (bm->next) bm->next->prev = bm->prev;
                    else bookmarks.tail = bm->prev;

                    delete bm;
                }
                bm = nextBm;
            }

            if (current->prev) current->prev->next = current->next;
            else history.head = current->next;

            if (current->next) current->next->prev = current->prev;
            else history.tail = current->prev;


            related_Pages* rp = current->rP_head;
            while (rp != nullptr) {
                related_Pages* nextRp = rp->next;
                delete rp;
                rp = nextRp;
            }

            delete current;
            cout << "Page " << pageID << " deleted successfully!" << endl;
            return;
        }
        current = current->next;
    }

    cout << "Page with ID " << pageID << " not found!" << endl;
}



bookMarkNode** getMostRecentBookmarks(bookMarkList& bookmarks, int n) {
    int count = 0;
    bookMarkNode* temp = bookmarks.head;
    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }

    if (n > count) n = count;
    if (n <= 0) return nullptr;

    bookMarkNode** result = new bookMarkNode * [n];


    for (int i = 0; i < n; i++) {
        bookMarkNode* mostRecent = nullptr;
        time_t maxTime = 0;

        temp = bookmarks.head;
        while (temp != nullptr) {

            bool alreadySelected = false;
            for (int j = 0; j < i; j++) {
                if (result[j] == temp) {
                    alreadySelected = true;
                    break;
                }
            }

            if (!alreadySelected && temp->lastVisited > maxTime) {
                maxTime = temp->lastVisited;
                mostRecent = temp;
            }

            temp = temp->next;
        }

        result[i] = mostRecent;
    }

    return result;
}

void searchByURL(pages_History& history, const string& substring) {
    bool found = false;
    Page* current = history.head;
    int resultCount = 0;

    cout << "\n========================================" << endl;
    cout << "Search results for \"" << substring << "\":" << endl;
    cout << "========================================" << endl;

    while (current != nullptr) {
        if (current->URL.find(substring) != string::npos) {
            resultCount++;
            cout << "\nResult #" << resultCount << ":" << endl;
            cout << "  ID: " << current->P_ID << endl;
            cout << "  URL: " << current->URL << endl;
            cout << "  Title: " << current->title << endl;
            cout << "  Visited: " << formatDate(current->visitedAt) << endl;
            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "\nNo pages found containing \"" << substring << "\"" << endl;
    }
    else {
        cout << "\nTotal: " << resultCount << " page(s) found" << endl;
    }
    cout << "========================================" << endl;
}


void removeLeastVisitedBookmark(bookMarkList& bookmarks) {
    if (bookmarks.head == nullptr) {
        cout << "No bookmarks to remove!" << endl;
        return;
    }

    bookMarkNode* leastVisited = bookmarks.head;
    int minVisits = leastVisited->visitCount;

    bookMarkNode* current = bookmarks.head->next;
    while (current != nullptr) {
        if (current->visitCount < minVisits) {
            minVisits = current->visitCount;
            leastVisited = current;
        }
        current = current->next;
    }

    if (leastVisited->prev) leastVisited->prev->next = leastVisited->next;
    else bookmarks.head = leastVisited->next;

    if (leastVisited->next) leastVisited->next->prev = leastVisited->prev;
    else bookmarks.tail = leastVisited->prev;

    cout << "Removed least visited bookmark: " << leastVisited->page->title
        << " (Visit count: " << minVisits << ")" << endl;

    delete leastVisited;
}


void writeToFile(const string& filename, pages_History& history, bookMarkList& bookmarks) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file for writing: " << filename << endl;
        return;
    }

    Page* current = history.head;
    while (current != nullptr) {

        file << "-" << current->P_ID << ", " << current->URL << ", "
            << current->title << ", " << formatDate(current->visitedAt) << endl;


        related_Pages* rp = current->rP_head;
        while (rp != nullptr) {
            file << "*" << rp->R_ID << ", " << rp->URL << ", " << rp->title << endl;
            rp = rp->next;
        }


        bookMarkNode* bm = bookmarks.head;
        while (bm != nullptr) {
            if (bm->page == current) {
                file << "#" << (bm->favorite ? "yes" : "no") << ", "
                    << bm->visitCount << ", " << formatDate(bm->lastVisited) << endl;
                break;
            }
            bm = bm->next;
        }

        current = current->next;
    }

    file.close();
    cout << "Data written to '" << filename << "' successfully!" << endl;
}

void displayAllPages(pages_History& history) {
    if (history.head == nullptr) {
        cout << "\nNo pages in history!" << endl;
        return;
    }

    Page* p = history.head;
    int count = 0;

    cout << "\n========================================" << endl;
    cout << "ALL PAGES IN HISTORY" << endl;
    cout << "========================================" << endl;

    while (p != nullptr) {
        count++;
        cout << "\nPage #" << count << ":" << endl;
        cout << "  ID: " << p->P_ID << endl;
        cout << "  Title: " << p->title << endl;
        cout << "  URL: " << p->URL << endl;
        cout << "  Visited: " << formatDate(p->visitedAt) << endl;


        int relCount = 0;
        related_Pages* rp = p->rP_head;
        while (rp != nullptr) {
            relCount++;
            rp = rp->next;
        }
        cout << "  Related Pages: " << relCount << endl;

        p = p->next;
    }

    cout << "========================================" << endl;
    cout << "Total: " << count << " page(s)" << endl;
    cout << "========================================" << endl;
}


void displayAllBookmarks(bookMarkList& bookmarks) {
    if (bookmarks.head == nullptr) {
        cout << "\nNo bookmarks!" << endl;
        return;
    }

    bookMarkNode* bm = bookmarks.head;
    int count = 0;

    cout << "\n========================================" << endl;
    cout << "ALL BOOKMARKS" << endl;
    cout << "========================================" << endl;

    while (bm != nullptr) {
        count++;
        cout << "\nBookmark #" << count << ":" << endl;
        cout << "  Page: " << bm->page->title << " (ID: " << bm->page->P_ID << ")" << endl;
        cout << "  URL: " << bm->page->URL << endl;
        cout << "  Favorite: " << (bm->favorite ? "Yes" : "No") << endl;
        cout << "  Visit Count: " << bm->visitCount << endl;
        cout << "  Last Visited: " << formatDate(bm->lastVisited) << endl;

        bm = bm->next;
    }

    cout << "========================================" << endl;
    cout << "Total: " << count << " bookmark(s)" << endl;
    cout << "========================================" << endl;
}


Page* findMostVisitedPage(pages_History& history, bookMarkList& bookmarks) {
    if (history.head == nullptr) {
        return nullptr;
    }

    Page* mostVisited = nullptr;
    int maxVisits = 0;

    Page* currentPage = history.head;
    while (currentPage != nullptr) {

        bookMarkNode* bm = bookmarks.head;
        while (bm != nullptr) {
            if (bm->page == currentPage) {
                if (bm->visitCount > maxVisits) {
                    maxVisits = bm->visitCount;
                    mostVisited = currentPage;
                }
                break;
            }
            bm = bm->next;
        }
        currentPage = currentPage->next;
    }

    return mostVisited;
}

void displayHistory(pages_History& history) {
    if (history.head == nullptr) {
        cout << "\nNo pages in history!" << endl;
        return;
    }


    int count = 0;
    Page* temp = history.head;
    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }


    Page** pages = new Page * [count];
    temp = history.head;
    for (int i = 0; i < count; i++) {
        pages[i] = temp;
        temp = temp->next;
    }


    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (pages[i]->visitedAt < pages[j]->visitedAt) {
                Page* swap = pages[i];
                pages[i] = pages[j];
                pages[j] = swap;
            }
        }
    }


    cout << "\n========================================" << endl;
    cout << "BROWSING HISTORY (Most Recent First)" << endl;
    cout << "========================================" << endl;
    for (int i = 0; i < count; i++) {
        cout << "\n" << (i + 1) << ". " << pages[i]->title << endl;
        cout << "   URL: " << pages[i]->URL << endl;
        cout << "   Visited: " << formatDate(pages[i]->visitedAt) << endl;
    }
    cout << "========================================" << endl;
    cout << "Total: " << count << " page(s)" << endl;
    cout << "========================================" << endl;

    delete[] pages;
}

void addBookmark(pages_History& history, bookMarkList& bookmarks) {
    if (history.head == nullptr) {
        cout << "\nNo pages in history to bookmark!" << endl;
        return;
    }

    int pageID;
    cout << "\nEnter the Page ID you want to bookmark: ";
    cin >> pageID;
    cin.ignore();


    Page* current = history.head;
    Page* targetPage = nullptr;
    while (current != nullptr) {
        if (current->P_ID == pageID) {
            targetPage = current;
            break;
        }
        current = current->next;
    }

    if (targetPage == nullptr) {
        cout << "Page with ID " << pageID << " not found!" << endl;
        return;
    }


    bookMarkNode* bm = bookmarks.head;
    while (bm != nullptr) {
        if (bm->page == targetPage) {
            cout << "This page is already bookmarked!" << endl;
            cout << "Current visit count: " << bm->visitCount << endl;
            cout << "Would you like to update it? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore();
            if (choice == 'y' || choice == 'Y') {
                bm->visitCount++;
                bm->lastVisited = time(0);
                cout << "Bookmark updated! New visit count: " << bm->visitCount << endl;
            }
            return;
        }
        bm = bm->next;
    }


    bookMarkNode* newBookmark = new bookMarkNode();
    newBookmark->page = targetPage;
    newBookmark->prev = newBookmark->next = nullptr;

    cout << "Mark as favorite? (y/n): ";
    char fav;
    cin >> fav;
    cin.ignore();
    newBookmark->favorite = (fav == 'y' || fav == 'Y');

    newBookmark->visitCount = 1;
    newBookmark->lastVisited = time(0);


    if (bookmarks.head == nullptr) {
        bookmarks.head = bookmarks.tail = newBookmark;
    }
    else {
        bookmarks.tail->next = newBookmark;
        newBookmark->prev = bookmarks.tail;
        bookmarks.tail = newBookmark;
    }

    cout << "\nBookmark added successfully!" << endl;
    cout << "  Page: " << targetPage->title << endl;
    cout << "  Favorite: " << (newBookmark->favorite ? "Yes" : "No") << endl;
}

int main() {
    pages_History history;
    bookMarkList bookmarks;
    history.head = history.tail = nullptr;
    bookmarks.head = bookmarks.tail = nullptr;

    int choice;
    bool fileLoaded = false;

    cout << "==========================================" << endl;
    cout << "  BROWSER HISTORY MANAGEMENT SYSTEM" << endl;
    cout << "==========================================" << endl;

    do {
        cout << "\n============ MAIN MENU ============" << endl;
        cout << "1.  Parse input file" << endl;
        cout << "2.  Create new page" << endl;
        cout << "3.  Remove pages older than date" << endl;
        cout << "4.  Find most related page" << endl;
        cout << "5.  Delete a page" << endl;
        cout << "6.  Get recent bookmarks" << endl;
        cout << "7.  Search by URL substring" << endl;
        cout << "8.  Remove least visited bookmark" << endl;
        cout << "9.  Write to output file" << endl;
        cout << "10. Display all pages" << endl;
        cout << "11. Display all bookmarks" << endl;
        cout << "12. Find most visited pages" << endl;
        cout << "13. Display history (chronologically)" << endl;
        cout << "14. Add bookmark" << endl;
        cout << "0.  Exit" << endl;
        cout << "===================================" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            string filename;
            cout << "Enter input filename: ";
            getline(cin, filename);
            parseInputFile(filename, history, bookmarks);
            fileLoaded = true;
            break;
        }

        case 2: {
            int id, numRelated;
            string url, title;

            cout << "\n--- Create New Page ---" << endl;
            cout << "Enter page ID: ";
            cin >> id;
            cin.ignore();
            cout << "Enter URL: ";
            getline(cin, url);
            cout << "Enter title: ";
            getline(cin, title);
            cout << "Enter number of related pages: ";
            cin >> numRelated;
            cin.ignore();

            int* ids = new int[numRelated];
            string* urls = new string[numRelated];
            string* titles = new string[numRelated];

            for (int i = 0; i < numRelated; i++) {
                cout << "\nRelated page " << (i + 1) << ":" << endl;
                cout << "  ID: ";
                cin >> ids[i];
                cin.ignore();
                cout << "  URL: ";
                getline(cin, urls[i]);
                cout << "  Title: ";
                getline(cin, titles[i]);
            }

            createPage(history, id, url, title, time(0), ids, urls, titles, numRelated);

            delete[] ids;
            delete[] urls;
            delete[] titles;
            break;
        }

        case 3: {
            string dateStr;
            cout << "\nEnter cutoff date (DD/MM/YYYY HH:MM): ";
            getline(cin, dateStr);
            time_t cutoff = parseDate(dateStr);
            removeOldPages(history, bookmarks, cutoff);
            break;
        }

        case 4: {
            Page* p = findMostRelatedPage(history);
            if (p) {
                cout << "\nMost related page found:" << endl;
                cout << "  Title: " << p->title << endl;
                cout << "  ID: " << p->P_ID << endl;
                cout << "  URL: " << p->URL << endl;


                int count = 0;
                related_Pages* rp = p->rP_head;
                while (rp != nullptr) {
                    count++;
                    rp = rp->next;
                }
                cout << "  Number of related pages: " << count << endl;
            }
            else {
                cout << "\nNo pages found in history!" << endl;
            }
            break;
        }

        case 5: {
            int id;
            cout << "\nEnter page ID to delete: ";
            cin >> id;
            deletePage(history, bookmarks, id);
            break;
        }

        case 6: {
            int n;
            cout << "\nEnter number of recent bookmarks to retrieve: ";
            cin >> n;

            bookMarkNode** recent = getMostRecentBookmarks(bookmarks, n);

            if (recent != nullptr) {
                cout << "\n========================================" << endl;
                cout << "Most recent bookmarks:" << endl;
                cout << "========================================" << endl;
                for (int i = 0; i < n && recent[i] != nullptr; i++) {
                    cout << (i + 1) << ". " << recent[i]->page->title << endl;
                    cout << "   URL: " << recent[i]->page->URL << endl;
                    cout << "   Last visited: " << formatDate(recent[i]->lastVisited) << endl;
                    cout << "   Visit count: " << recent[i]->visitCount << endl;
                    cout << "----------------------------------------" << endl;
                }
                delete[] recent;
            }
            else {
                cout << "\nNo bookmarks available!" << endl;
            }
            break;
        }

        case 7: {
            string substring;
            cout << "\nEnter URL substring to search: ";
            getline(cin, substring);
            searchByURL(history, substring);
            break;
        }

        case 8: {
            removeLeastVisitedBookmark(bookmarks);
            break;
        }

        case 9: {
            string filename;
            cout << "Enter output filename: ";
            getline(cin, filename);
            writeToFile(filename, history, bookmarks);
            break;
        }

        case 10: {
            displayAllPages(history);
            break;
        }

        case 11: {
            displayAllBookmarks(bookmarks);
            break;
        }
        case 12: {
            Page* p = findMostVisitedPage(history, bookmarks);
            if (p) {
                cout << "\nMost visited page found:" << endl;
                cout << "  Title: " << p->title << endl;
                cout << "  ID: " << p->P_ID << endl;
                cout << "  URL: " << p->URL << endl;


                bookMarkNode* bm = bookmarks.head;
                while (bm != nullptr) {
                    if (bm->page == p) {
                        cout << "  Visit count: " << bm->visitCount << endl;
                        break;
                    }
                    bm = bm->next;
                }
            }
            else {
                cout << "\nNo visited pages found in bookmarks!" << endl;
            }
            break;
        }

        case 13: {
            displayHistory(history);
            break;
        }

        case 14: {
            addBookmark(history, bookmarks);
            break;
        }

        case 0:
            cout << "\nExiting program. Goodbye!" << endl;
            break;

        default:
            cout << "\nInvalid choice! Please enter a number between 0-14." << endl;
        }

    } while (choice != 0);

    Page* p = history.head;
    while (p != nullptr) {
        Page* nextPage = p->next;
        related_Pages* rp = p->rP_head;
        while (rp != nullptr) {
            related_Pages* nextRp = rp->next;
            delete rp;
            rp = nextRp;
        }
        delete p;
        p = nextPage;
    }

    bookMarkNode* bm = bookmarks.head;
    while (bm != nullptr) {
        bookMarkNode* nextBm = bm->next;
        delete bm;
        bm = nextBm;
    }

    return 0;
}