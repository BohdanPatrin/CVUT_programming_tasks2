#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <compare>
#include <functional>
#include <optional>

class CTimeStamp
{
public:
    CTimeStamp(int year, int month, int day, int hour, int minute, double sec)
            : m_Year(year), m_Month(month), m_Day(day),
              m_Hour(hour), m_Minute(minute), m_Second(sec) {}

    int compare(const CTimeStamp &x) const {
        if (m_Year != x.m_Year) return m_Year - x.m_Year;
        if (m_Month != x.m_Month) return m_Month - x.m_Month;
        if (m_Day != x.m_Day) return m_Day - x.m_Day;
        if (m_Hour != x.m_Hour) return m_Hour - x.m_Hour;
        if (m_Minute != x.m_Minute) return m_Minute - x.m_Minute;
        if (m_Second != x.m_Second) return (m_Second > x.m_Second) ? 1 : -1;
        return 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const CTimeStamp &x) {
        int sec = static_cast<int>(x.m_Second);
        int usec = static_cast<int>((x.m_Second - sec) * 1'000'000 + 0.5);

        os << std::setfill('0')
           << std::setw(4) << x.m_Year << '-'
           << std::setw(2) << x.m_Month << '-'
           << std::setw(2) << x.m_Day << ' '
           << std::setw(2) << x.m_Hour << ':'
           << std::setw(2) << x.m_Minute << ':'
           << std::setw(2) << sec << '.'
           << std::setw(3) << (usec / 1000); // display milliseconds only
        return os;
    }

private:
    int m_Year;
    int m_Month;
    int m_Day;
    int m_Hour;
    int m_Minute;
    double m_Second; // contains seconds + microseconds (e.g., 12.345678)
};
class CMail
{
public:
    CMail(const CTimeStamp &timeStamp,
          const std::string &from,
          const std::string &to,
          const std::optional<std::string> &subject)
            : m_TimeStamp(timeStamp),
              m_From(from),
              m_To(to),
              m_Subject(subject) {}

    int compareByTime(const CTimeStamp &x) const {
        return m_TimeStamp.compare(x);
    }

    int compareByTime(const CMail &x) const {
        return m_TimeStamp.compare(x.m_TimeStamp);
    }

    const std::string &from() const {
        return m_From;
    }

    const std::string &to() const {
        return m_To;
    }

    const std::optional<std::string> &subject() const {
        return m_Subject;
    }

    const CTimeStamp &timeStamp() const {
        return m_TimeStamp;
    }

    friend std::ostream &operator<<(std::ostream &os, const CMail &x) {
        os << x.m_TimeStamp << " " << x.m_From << " -> " << x.m_To;
        if (x.m_Subject.has_value()) os << ", subject: " << x.m_Subject.value();
        return os;
    }

private:
    CTimeStamp m_TimeStamp;
    std::string m_From;
    std::string m_To;
    std::optional<std::string> m_Subject;
};
// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */
//----------------------------------------------------------------------------------------
    struct TempMailData {
        std::string from;
        std::optional<std::string> subject;
    };


    class CMailLog
    {
    public:
        CMailLog() = default;

        ~CMailLog() = default;

        int                   parseLog      ( std::istream     & in ){
            std::string line;
            int res = 0;

            while (std::getline(in, line)) {
                std::istringstream iss(line);

                std::string monthStr;
                int day, year, hour, minute;
                double second;

                iss >> monthStr >> day >> year >> hour;
                iss.ignore(1); // skip ':'
                iss >> minute;
                iss.ignore(1); // skip ':'
                iss >> second;

                // convert month name to number
                static const std::map<std::string, int> monthMap = {
                        { "Jan", 1 }, { "Feb", 2 }, { "Mar", 3 }, { "Apr", 4 },
                        { "May", 5 }, { "Jun", 6 }, { "Jul", 7 }, { "Aug", 8 },
                        { "Sep", 9 }, { "Oct", 10 }, { "Nov", 11 }, { "Dec", 12 }
                };
                int month = monthMap.at(monthStr); // assume valid input

                CTimeStamp ts(year, month, day, hour, minute, second);


                std::string hostname;
                iss >> hostname;


                std::string rest;
                iss >> rest;

                size_t colonPos = rest.find(':');
                if (colonPos == std::string::npos) continue;

                std::string msgID = rest.substr(0, colonPos);
                std::string contentType;
                std::getline(iss, contentType);


                contentType.erase(0, contentType.find_first_not_of(" \t"));
                if (contentType.starts_with("from=")) {
                    TempMailData msg;
                    msg.from = contentType.substr(5);
                    in_progress.insert(std::make_pair(msgID, msg));
                    continue;
                }
                else if (contentType.starts_with("subject=")) {
                    std::string subject = contentType.substr(8);
                    auto it = in_progress.find(msgID);
                    if (it == in_progress.end()) continue; // messageId not found — maybe 'from=' was missing or log is malformed

                    TempMailData & data = it->second;
                    data.subject = subject;
                    continue;
                }
                else if (contentType.starts_with("to=")) {
                    std::string recipient = contentType.substr(3);
                    auto it = in_progress.find(msgID);
                    if (it == in_progress.end()) continue; // messageId not found — maybe 'from=' was missing or log is malformed
                    CMail rec(ts, it->second.from, recipient, it->second.subject);
                    if(mails.empty()) {
                        mails.push_back(rec);
                        res++;
                        continue;
                    }
                    auto pos = std::lower_bound(mails.begin(), mails.end(), rec, [](const CMail &a, const CMail &b) {
                        return a.compareByTime(b) <= 0;
                    });

                    mails.insert(pos, rec);
                    res++;
                    continue;
                }
            }
            return res;
        }

        std::list<CMail>      listMail      ( const CTimeStamp & from,
                                              const CTimeStamp & to ) const{
            CMail begin(from, "", "", "");
            CMail end(to, "", "", "");

            auto lower = std::lower_bound(mails.begin(), mails.end(), begin, [](const CMail &a, const CMail &b) {
                return a.compareByTime(b) < 0;
            });
            auto upper = std::upper_bound(mails.begin(), mails.end(), end, [](const CMail &a, const CMail &b) {
                return a.compareByTime(b) < 0;
            });

            return std::list<CMail>(lower, upper);
        }


        std::set<std::string> activeUsers   ( const CTimeStamp & from,
                                              const CTimeStamp & to ) const{
            std::set<std::string> result;
            std::list<CMail> range = listMail(from, to);

            for (const CMail& mail : range) {
                result.insert(mail.from());
                result.insert(mail.to());
            }

            return result;
        }
    private:
        // todo
        std::vector<CMail> mails; // Final list of complete CMail records, always sorted by timestamp
        std::unordered_map<std::string, TempMailData> in_progress; // keyed by message ID
    };
//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
std::string             printMail       ( const std::list<CMail> & all )
{
    std::ostringstream oss;
    for ( const auto & mail : all )
        oss << mail << "\n";
    return oss . str ();
}
int                     main ()
{
    MysteriousNamespace::CMailLog m;
    std::list<CMail> mailList;
    std::set<std::string> users;
    std::istringstream iss;

    iss . clear ();
    iss . str (
            "Mar 29 2025 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
            "Mar 29 2025 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
            "Mar 29 2025 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
            "Mar 29 2025 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
            "Mar 29 2025 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
            "Mar 29 2025 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
            "Mar 29 2025 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
            "Mar 29 2025 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
            "Mar 29 2025 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
            "Mar 29 2025 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
            "Mar 29 2025 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
            "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
            "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
            "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
            "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n" );
    assert ( m . parseLog ( iss ) == 8 );
    mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
    assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner
)###" );
    mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2025, 3, 29, 14, 58, 32 ) );
    assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
)###" );
    mailList = m . listMail ( CTimeStamp ( 2025, 3, 30, 0, 0, 0 ),
                              CTimeStamp ( 2025, 3, 30, 23, 59, 59 ) );
    assert ( printMail ( mailList ) == "" );
    users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
    assert ( users == std::set<std::string>( { "CEO@fit.cvut.cz", "CIO@fit.cvut.cz", "HR-department@fit.cvut.cz", "PR-department@fit.cvut.cz", "archive@fit.cvut.cz", "boss13@fit.cvut.cz", "dean@fit.cvut.cz", "office13@fit.cvut.cz", "person3@fit.cvut.cz", "user76@fit.cvut.cz", "vice-dean@fit.cvut.cz" } ) );
    users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                              CTimeStamp ( 2025, 3, 29, 13, 59, 59 ) );
    assert ( users == std::set<std::string>( { "person3@fit.cvut.cz", "user76@fit.cvut.cz" } ) );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
