/*
202418942
Dinh The Minh
*/
#include <bits/stdc++.h>
using namespace std;

class Employee {
private:
    void setBaseSalaryChecked(double newValue) {
        if (!isfinite(newValue))
            throw overflow_error("Salary overflow");
        baseSalary_ = newValue;
    }

    string id_;
    string fullName_;
    double baseSalary_;
    
public:
    Employee() : Employee("UNKNOWN", "Unnamed employee", 0.0) {}

    Employee(const string& id, const string& fullName)
        : Employee(id, fullName, 0.0) {}

    Employee(const string& id, const string& fullName, double baseSalary)
        : id_(id), fullName_(fullName), baseSalary_(baseSalary) {
        requireNonEmpty(id_, "Employee id");
        requireNonEmpty(fullName_, "Employee fullName");
        requireNonNegative(baseSalary_, "Employee baseSalary");
    }

    virtual ~Employee() {
        cout << "  [~Employee] " << id_ << " destroyed\n";
    }

    void increaseSalary(double amount) {
        requirePositive(amount, "increaseSalary amount");
        setBaseSalaryChecked(baseSalary_ + amount);
    }

    void increaseSalary(double value, bool byPercentage) {
        requirePositive(value, "increaseSalary value");
        if (byPercentage)
            setBaseSalaryChecked(baseSalary_ + baseSalary_ * value / 100.0);
        else
            setBaseSalaryChecked(baseSalary_ + value);
    }

    const string& getId() const { return id_; }
    const string& getFullName() const { return fullName_; }
    double getBaseSalary() const { return baseSalary_; }

    virtual double calculateMonthlyCost() const { return baseSalary_; }

    virtual void displayInfo() const {
        cout << "Employee | id=" << id_
             << " | name=" << fullName_
             << " | baseSalary=" << money(baseSalary_)
             << " | monthlyCost=" << money(calculateMonthlyCost()) << '\n';
    }

protected:
    static void requireNonEmpty(const string& s, const char* what) {
        if (s.empty())
            throw invalid_argument(string(what) + " must not be empty");
    }
    static void requireNonNegative(double x, const char* what) {
        if (!isfinite(x) || x < 0.0)
            throw invalid_argument(string(what) + " must be finite and non-negative");
    }
    static void requirePositive(double x, const char* what) {
        if (!isfinite(x) || x <= 0.0)
            throw invalid_argument(string(what) + " must be finite and positive");
    }
    static string money(double x) {
        ostringstream os;
        os << fixed << setprecision(2) << x;
        return os.str();
    }
};

class SoftwareEngineer : public Employee {

private:
    string primaryLanguage_;
    double technicalAllowance_;

public:
    SoftwareEngineer(const string& id, const string& fullName,
                     const string& primaryLanguage)
        : SoftwareEngineer(id, fullName, 0.0, primaryLanguage, 0.0) {}

    SoftwareEngineer(const string& id, const string& fullName,
                     double baseSalary, const string& primaryLanguage,
                     double technicalAllowance)
        : Employee(id, fullName, baseSalary),
          primaryLanguage_(primaryLanguage),
          technicalAllowance_(technicalAllowance) {
        requireNonEmpty(primaryLanguage_, "SoftwareEngineer primaryLanguage");
        requireNonNegative(technicalAllowance_, "SoftwareEngineer technicalAllowance");
    }

    ~SoftwareEngineer() override {
        cout << "  [~SoftwareEngineer] " << getId() << " destroyed\n";
    }

    const string& getPrimaryLanguage() const { return primaryLanguage_; }
    double getTechnicalAllowance() const { return technicalAllowance_; }

    double calculateMonthlyCost() const override {
        return getBaseSalary() + technicalAllowance_;
    }

    void displayInfo() const override {
        cout << "SoftwareEngineer | id=" << getId()
             << " | name=" << getFullName()
             << " | language=" << primaryLanguage_
             << " | baseSalary=" << money(getBaseSalary())
             << " | allowance=" << money(technicalAllowance_)
             << " | monthlyCost=" << money(calculateMonthlyCost()) << '\n';
    }

};

class ProjectTeam {
private:
    void requireNonEmpty() const {
        if (projectCode_.empty())
            throw invalid_argument("ProjectTeam projectCode must not be empty");
        if (projectName_.empty())
            throw invalid_argument("ProjectTeam projectName must not be empty");
    }

    vector<Employee*>::const_iterator findById(const string& id) const {
        auto it = members_.begin();
        for (; it != members_.end(); ++it)
            if ((*it)->getId() == id) break;
        return it;
    }
    vector<Employee*>::iterator findById(const string& id) {
        auto it = members_.begin();
        for (; it != members_.end(); ++it)
            if ((*it)->getId() == id) break;
        return it;
    }

    string projectCode_;
    string projectName_;
    Employee* leader_;
    vector<Employee*> members_;

public:
    ProjectTeam(const string& projectCode, const string& projectName)
        : projectCode_(projectCode), projectName_(projectName), leader_(nullptr) {
        requireNonEmpty();
    }

    ProjectTeam(const string& projectCode, const string& projectName,
                Employee& leader)
        : projectCode_(projectCode), projectName_(projectName), leader_(&leader) {
        requireNonEmpty();
        members_.push_back(&leader);
    }

    ~ProjectTeam() {
        cout << "  [~ProjectTeam] " << projectCode_
             << " destroyed (" << members_.size() << " links released)\n";
    }

    bool addMember(Employee& employee) { return addMember(employee, false); }

    bool addMember(Employee& employee, bool makeLeader) {
        if (contains(employee.getId())) return false;
        members_.push_back(&employee);
        if (makeLeader) leader_ = &employee;
        return true;
    }

    bool removeMember(const string& employeeId) {
        auto it = findById(employeeId);
        if (it == members_.end()) return false;
        if (*it == leader_) return false;
        members_.erase(it);
        return true;
    }

    bool changeLeader(Employee& employee) {
        auto it = findById(employee.getId());
        if (it != members_.end() && *it != &employee) return false;
        if (it == members_.end()) members_.push_back(&employee);
        leader_ = &employee;
        return true;
    }

    bool contains(const string& employeeId) const {
        return findById(employeeId) != members_.end();
    }

    double calculateTotalMonthlyCost() const {
        double total = 0.0;
        for (const Employee* e : members_) total += e->calculateMonthlyCost();
        return total;
    }

    void displayTeam() const {
        cout << "Project " << projectCode_ << " - " << projectName_ << '\n';
        cout << "Leader: "
             << (leader_ ? leader_->getId() + " (" + leader_->getFullName() + ")"
                         : string("(none)"))
             << '\n';
        cout << "Members (" << members_.size() << "):\n";
        for (const Employee* e : members_) {
            cout << (e == leader_ ? "  * " : "    ");
            e->displayInfo();
        }
        cout << "Total monthly cost: " << fixed << setprecision(2)
             << calculateTotalMonthlyCost() << '\n';
        cout.unsetf(ios::floatfield);
        cout << setprecision(6);
    }

    const string& getProjectCode() const { return projectCode_; }
    const string& getProjectName() const { return projectName_; }
    const Employee* getLeader() const { return leader_; }
    size_t size() const { return members_.size(); }

};

namespace {
int g_failures = 0;
int g_checks = 0;

void check(bool cond, const string& label) {
    ++g_checks;
    if (!cond) ++g_failures;
    cout << (cond ? "  [PASS] " : "  [FAIL] ") << label << '\n';
}

bool near(double a, double b) { return fabs(a - b) < 1e-6; }

template <class F>
bool throwsInvalid(F f) {
    try { f(); } catch (const invalid_argument&) { return true; }
    catch (...) { return false; }
    return false;
}

}

int main() {
    Employee e1("E001", "Nguyen Van A");
    Employee e2("E002", "Tran Thi B", 15000000.0);
    e1.displayInfo();
    e2.displayInfo();
    check(e1.getBaseSalary() == 0.0, "2-arg constructor: baseSalary defaults to 0");
    check(near(e2.getBaseSalary(), 15000000.0), "3-arg constructor: baseSalary stored");

    SoftwareEngineer s1("S001", "Le Van C", "C++");
    SoftwareEngineer s2("S002", "Pham Thi D", 30000000.0, "Java", 5000000.0);
    s1.displayInfo();
    s2.displayInfo();
    check(near(s1.calculateMonthlyCost(), 0.0), "S001 cost = 0 + 0");
    check(near(s2.calculateMonthlyCost(), 35000000.0), "S002 cost = base + allowance");

    e1.increaseSalary(2000000.0);
    check(near(e1.getBaseSalary(), 2000000.0), "E001 base = 2,000,000");

    e2.increaseSalary(10.0, true);
    check(near(e2.getBaseSalary(), 16500000.0), "E002 base = 15,000,000 * 1.10");
    e1.increaseSalary(500000.0, false);
    check(near(e1.getBaseSalary(), 2500000.0), "increaseSalary(v,false) is a fixed amount");

    ProjectTeam t1("P01", "Alpha");
    check(t1.getLeader() == nullptr, "no leader");
    check(t1.size() == 0, "empty");

    check(t1.addMember(e1), "add E001");

    check(t1.addMember(s2, true), "add S002 as leader");
    check(t1.getLeader() == &s2, "leader is S002");
    check(t1.addMember(e2), "add E002");
    check(t1.addMember(s1), "add S001");

    check(!t1.addMember(e1), "re-add E001 rejected");
    check(!t1.addMember(s2, true), "re-add with makeLeader refused too");
    Employee impostor("E002", "Different object, same id");
    check(!t1.addMember(impostor), "different object with existing id rejected");
    check(t1.size() == 4, "size unchanged (4)");
    check(t1.getLeader() == &s2, "refused calls leave the leader untouched");

    t1.displayTeam();

    check(near(t1.calculateTotalMonthlyCost(), 54000000.0), "total = 54,000,000");

    check(!t1.removeMember("S002"), "remove leader rejected");
    check(t1.contains("S002"), "S002 still a member");
    check(t1.getLeader() == &s2, "leader unchanged");

    check(t1.changeLeader(s1), "leader -> S001 (member promoted by changeLeader)");
    check(t1.getLeader() == &s1, "leader is S001");
    check(t1.removeMember("S002"), "remove S002");
    check(!t1.contains("S002"), "S002 gone from team");
    check(!t1.removeMember("S002"), "second removal returns false");
    check(t1.size() == 3, "size = 3");

    {
        ProjectTeam t2("P02", "Beta");
        check(t2.addMember(e1), "E001 added to second team");
        check(t1.contains("E001") && t2.contains("E001"),
              "E001 is a member of both teams at the same time");
        check(near(t2.calculateTotalMonthlyCost(), e1.getBaseSalary()),
              "t2 cost counts only its own member");
        t2.displayTeam();

        cout << "  (end of scope: t2 destructor runs now)\n";
    }

    check(t1.contains("E001"), "t1 still holds E001 after t2 was destroyed");
    check(e1.getId() == "E001" && near(e1.getBaseSalary(), 2500000.0),
          "E001 untouched by the destruction of t2");
    e1.displayInfo();

    {
        Employee d;
        check(d.getId() == "UNKNOWN" && d.getFullName() == "Unnamed employee"
                  && d.getBaseSalary() == 0.0,
              "default Employee values");
    }
    check(throwsInvalid([] { Employee x("", "n"); }), "empty id rejected");
    check(throwsInvalid([] { Employee x("i", ""); }), "empty name rejected");
    check(throwsInvalid([] { Employee x("i", "n", -1.0); }), "negative salary rejected");
    check(throwsInvalid([] { Employee x("i", "n", NAN); }), "NaN salary rejected");
    {
        Employee z("Z", "Zero", 0.0);
        check(true, "zero salary accepted");
        check(throwsInvalid([&] { z.increaseSalary(0.0); }), "increase by 0 rejected");
        check(throwsInvalid([&] { z.increaseSalary(-5.0); }), "negative increase rejected");
        check(throwsInvalid([&] { z.increaseSalary(0.0, true); }), "0 percent rejected");
        check(throwsInvalid([&] { z.increaseSalary(-5.0, false); }), "negative fixed (bool) rejected");
        check(near(z.getBaseSalary(), 0.0), "failed raises leave salary unchanged");
        z.increaseSalary(10.0, true);
        check(near(z.getBaseSalary(), 0.0), "percentage of 0 leaves 0");
    }
    check(throwsInvalid([] { SoftwareEngineer x("i", "n", ""); }), "empty language rejected");
    check(throwsInvalid([] { SoftwareEngineer x("i", "n", 1.0, "C++", -1.0); }),
          "negative allowance rejected");
    check(throwsInvalid([] { SoftwareEngineer x("", "n", "C++"); }), "SE: empty id rejected");

    check(throwsInvalid([] { ProjectTeam x("", "n"); }), "empty project code rejected");
    check(throwsInvalid([] { ProjectTeam x("c", ""); }), "empty project name rejected");
    {
        Employee a("A1", "Alice", 100.0), b("B1", "Bob", 200.0);
        ProjectTeam t("P9", "Gamma", a);
        check(t.size() == 1 && t.contains("A1") && t.getLeader() == &a,
              "leader constructor: leader is also a member");
        check(near(t.calculateTotalMonthlyCost(), 100.0), "leader counted once");
        check(!t.removeMember("A1"), "cannot remove sole leader");
        check(!t.removeMember("nope"), "unknown id: false");
        check(!t.removeMember(""), "empty id: false");
        check(t.changeLeader(b), "changeLeader adds non-member");
        check(t.contains("B1") && t.getLeader() == &b && t.size() == 2,
              "new leader added and set");
        check(t.changeLeader(b), "changeLeader to current leader is a no-op");
        check(t.size() == 2, "no duplicate created");
        Employee b2("B1", "Other Bob", 1.0);
        check(!t.changeLeader(b2), "changeLeader with id clash rejected");
        check(t.getLeader() == &b, "leader unchanged after rejection");
        check(t.removeMember("A1"), "former leader removable after change");
        check(t.size() == 1, "size = 1");
    }
    {
        ProjectTeam empty("E", "Empty");
        check(near(empty.calculateTotalMonthlyCost(), 0.0), "empty team cost = 0");
        empty.displayTeam();
        Employee a("A2", "Ann", 10.0);
        check(empty.addMember(a, true), "first member as leader");
        check(empty.getLeader() == &a, "leader set");
        check(!empty.addMember(a, true), "promotion of a member is changeLeader's job");
        check(empty.size() == 1 && empty.getLeader() == &a, "refusal changed nothing");
    }

    cout << "\n=== " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed ===\n";
    cout << "\n(destructors of remaining objects follow)\n";
    return g_failures == 0 ? 0 : 1;
}
