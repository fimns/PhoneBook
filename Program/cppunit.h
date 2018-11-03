#ifndef __JSS_CPPUNIT_H__
#define __JSS_CPPUNIT_H__
#include <iostream>  // for streams
#include <vector>    // for vector
#include <string>    // for string
#include <stdexcept> // for exception
#include <sstream>   // for stringstream
#include <cassert>   // for assert macro
#include <ctime>     // for time-functions
#include <iomanip>   // setw() etc

namespace jss
{

        namespace cppunit
        {

                ///-----------------------------------------------------------------------------------------------------
                /// @brief Timer Class
                ///-----------------------------------------------------------------------------------------------------
                /// 테스트 수행시 걸린 시간을 측정하기 위해 사용되는 간단한 Util 클래스.
                /// start(), stop()을 반복적으로 호출할 수 있으며 전체 시간이 계속 누적된다. 다시 초기화를 하고자
                /// 한다면 clear() 함수를 사용하여야 한다. 총 수행 시간은 records()함수로 얻어지며 초 단위이다.
                ///-----------------------------------------------------------------------------------------------------
                class Timer
                {
                private :
                        double  m_total ;
                        clock_t m_stime ;
                        clock_t m_etime ;
                        volatile bool m_run ;
                public  :

                        Timer() : m_total(0.0), m_run(false)
                        {
                        }

                        void start()
                        {
                                m_stime = ::clock() ;
                                m_run   = true ;
                        }
                        void stop()
                        {
                                if ( false == m_run )
                                        return ;
                                double diff ;
                                m_etime = ::clock() ;
                                m_run   = false ;
                                diff = static_cast<double>(m_etime-m_stime) ;
                                m_total += diff/CLOCKS_PER_SEC ;
                                assert ( m_total >= 0.0 ) ;
                        }
                        void clear()
                        {
                                m_total = 0.0 ;
                                m_run   = false ;
                        }
                        double records() const
                        {
                                return m_total ;
                        }
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief Context Class
                ///-----------------------------------------------------------------------------------------------------
                /// 코드의 위치 정보를 담고 있는 클래스
                /// POD(Plain Old Data) 타입의 간단한 클래스. 코드 전반에서 사용된다.
                ///-----------------------------------------------------------------------------------------------------
                class Context
                {
                public  :
                        std::string m_msgs ;
                        std::string m_file ;
                        std::string m_func ;
                        std::string m_info ;
                        long        m_line ;
                public  :
                        Context() : m_msgs(), m_file(), m_func(), m_info(), m_line(0)
                        {
                        }

                        Context(const std::string& msgs, const std::string& file,
                                const std::string& func, const std::string& info, long line)
                        : m_msgs(msgs), m_file(file), m_func(func), m_info(info), m_line(line)
                        {
                        }
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief FailureException 클래스
                ///-----------------------------------------------------------------------------------------------------
                /// 단위 테스트에서 사용되는 assert 계열 함수가 실패하면 예외가 발생하는데, 이 때 발생되는 예외 객체가
                /// FailureException 객체이다. 실패시 해당 위치의 정보들을 함께 포함하여 예외로 반환되게 된다.
                ///-----------------------------------------------------------------------------------------------------
                class FailureException : public std::exception
                {
                private :
                        Context m_context ; // 위치 정보
                public  :
                        /// @brief 생성자
                        FailureException() : m_context()
                        {
                        }
                        /// @brief 복사 생성자
                        FailureException(const Context& context) : m_context(context)
                        {
                        }
                        /// @brief 소멸자
                        ~FailureException() throw()
                        {
                        }
                        /// @brief 에러 메세지 반환
                        const char* what() const throw()
                        {
                                return m_context.m_msgs.c_str() ;
                        }
                        /// @brief 현재 설정된 Context 정보를 반환한다.
                        /// Context에는 코드 위치 및 관련 메시지를 포함하고 있다.
                        const Context& context() const throw()
                        {
                                return m_context ;
                        }
                } ;






                ///-----------------------------------------------------------------------------------------------------
                /// @brief Method-Object Interface
                ///-----------------------------------------------------------------------------------------------------
                /// 사용자가 실제 테스트에 사용할 메서드들을 등록하게 되는데, 이를 내부에서는 하나의 객체 정보로
                /// 저장하여 사용하게 된다. 이를 위해 사용하는 Method 객체의 인터페이스로 실제 사용시에는 template
                /// 기반으로 구현되어 사용되게 된다. Concrete 객체의 사용법을 참고하도록 한다.
                ///-----------------------------------------------------------------------------------------------------
                class Method
                {
                public  :
                        virtual ~Method() { }
                        virtual void execute() const = 0 ;     // 등록된 메소드를 실행한다.
                        virtual Method* clone() const = 0 ;    // 현재 객체를 복사한다.(deep copy)
                        virtual const char* name() const = 0 ; // 현재 등록된 메소드의 이름을 반환한다.
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief Method Class Holder
                ///-----------------------------------------------------------------------------------------------------
                /// 함수 호출을 대행해주는 클래스로 실제 객체와 그 객체가 가지고 있는 메서드를 템플릿 파라미터로
                /// 입력받아 호출을 해주는 클래스이다. 간단한 Command 패턴 객체라 생각하면 된다.
                /// 이를 통해 메서드의 호출 시점을 이 클래스가 제어할 수 있다. ( Command 패턴의 장점 )
                /// 템플릿을 사용하는 이유는 Test Framework 에서는 사용 시점에서 어떤 클래스의 어떤 메서드를 사용할지
                /// 판단하기 어렵기 때문이다.
                ///-----------------------------------------------------------------------------------------------------
                template < class T, class TestMethod >
                class MethodHolder : public Method
                {
                private :
                        std::string m_name   ; // 테스트 메서드 이름 ( 메시지 출력을 위해 사용 )
                        T&          m_object ; // 테스트 메서드를 소유한 실제 객체
                        TestMethod  m_method ; // 테스트 메서드를 Wrapping 한 메서드 객체

                public  :
                        /// @brief MethodHolder 생성자
                        MethodHolder(const std::string& name, T& object, const TestMethod& testMethod)
                        : m_name(name), m_object(object), m_method(testMethod)
                        {
                        }

                        /// @brief MethodHolder 생성자
                        MethodHolder(T& object, const TestMethod& testMethod)
                        : m_name(""), m_object(object), m_method(testMethod)
                        {
                        }

                        /// @brief MethodHolder 복사 생성자
                        MethodHolder(const MethodHolder& holder)
                        : m_name(holder.m_name), m_object(holder.m_object), m_method(holder.m_method)
                        {
                        }

                        /// @brief 실제 함수를 호출하는 메서드.
                        /// Command 패턴의 execute() 역할을 한다.
                        void execute() const
                        {
                                (m_object.*m_method)() ;
                        }

                        /// @brief 함수 이름을 반환하는 메서드
                        const char* name() const throw()
                        {
                                return m_name.c_str() ;
                        }

                        /// @brief 복사 메서드
                        Method* clone() const
                        {
                                return new MethodHolder(*this) ;
                        }

                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief TestResult 클래스
                ///-----------------------------------------------------------------------------------------------------
                /// 테스트의 결과를 담아내는 클래스 UnitTest 에서 사용하는 TestResult와 하는 역할이 같다.
                /// 테스트 결과를 구동시키고, 그에 따른 테스트 결과들을 수집한다.
                /// 따라서 Test 객체들을 넘겨받아 구동(Trigger)시키는 모듈이 이 클래스에 포함되어 있다.
                ///-----------------------------------------------------------------------------------------------------
                class TestResult
                {
                private :
                        Timer m_timer ; // 시간 측정용 객체

                        std::vector<Context> m_success ; // 성공한 테스트 정보 객체
                        std::vector<Context> m_failure ; // 실패한 테스트 정보 객체
                        std::vector<Context> m_error   ; // 에러 테스트 정보 객체
                private :
                        void addSuccess(const Context& ctxt) { m_success.push_back(ctxt) ; }
                        void addFailure(const Context& ctxt) { m_failure.push_back(ctxt) ; }
                        void addError  (const Context& ctxt) { m_error.push_back(ctxt)   ; }
                public  :
                        // Helper Method
                        int totalRunCount() const { return (successCount() + errorCount() + failureCount()) ; }
                        int successCount()  const { return static_cast<int>(m_success.size()); }
                        int failureCount()  const { return static_cast<int>(m_failure.size()); }
                        int errorCount()    const { return static_cast<int>(m_error.size())  ; }

                        /// @brief 현재 테스트가 모두 성공적이었는지를 체크하는 메서드
                        bool wasSuccessful() const
                        {
                                if ( 0 == errorCount() && 0 == failureCount() )
                                        return true ;
                                return false ;
                        }

                        /// @brief 테스트 결과 초기화
                        void clear()
                        {
                                m_timer.clear()   ;
                                m_success.clear() ;
                                m_failure.clear() ;
                                m_error.clear()   ;
                        }

                        /// @brief 테스트 구동 함수. 하나의 메서드에 대해서 구동을 하게 된다.
                        bool run(const Method& method)
                        {
                                m_timer.start() ;
                                try
                                {
                                        method.execute() ; // 넘겨받은 테스트 함수를 호출한다.

                                        Context c("OK","",method.name(),"",0) ;
                                        addSuccess(c) ;
                                }
                                catch(FailureException& e)
                                {
                                        addFailure(e.context()) ;
                                }
                                catch(std::exception& e)
                                {
                                        Context c(e.what(),"",method.name(),"",0) ;
                                        addError(c) ;
                                }
                                catch(...)
                                {
                                        Context c("unexpected exception is occured","",method.name(),"",0) ;
                                        addError(c) ;
                                }
                                m_timer.stop() ;
                                return true ;
                        }

                        // 테스트 결과를 ostream 에 출력해주는 Helper 함수
                        friend std::ostream& operator<<(std::ostream& os, const TestResult& test) ;
                } ;




                ///-----------------------------------------------------------------------------------------------------
                /// @brief Test 인터페이스
                ///-----------------------------------------------------------------------------------------------------
                /// 사용자가 사용할 테스트 객체의 인터페이스
                ///-----------------------------------------------------------------------------------------------------
                class Test
                {
                public  :
                        virtual ~Test() { }
                        virtual void run(TestResult& result) = 0 ;
                        virtual int  countTestCase() const { return 1 ; }
                        virtual Test* clone() const = 0 ;

                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief TestCase 클래스
                ///-----------------------------------------------------------------------------------------------------
                /// 사용자가 작성할 하나의 테스트를 의미한다.
                /// UnitTest 프레임워크의 TestCase 와 그 기능이 동일하다. 그러나 C++에서는 리플렉션(reflection) 기능이
                /// 언어 Level 에서 지원하지 않기 때문에 사용법이 약간 다르다. 여기서는 매크로를 사용한다.
                ///-----------------------------------------------------------------------------------------------------
                #define append(function) methodAppend(#function, *this, function)
                #define _append(function)

                class TestCase : public Test
                {
                public  :
                        typedef std::vector<Method*> MethodList ;
                private :
                        MethodList m_methods ;
                protected :

                        TestCase() { }

                        TestCase(const TestCase& t)
                        {
                                MethodList::const_iterator cursor ;
                                for ( cursor=t.m_methods.begin() ; cursor!=t.m_methods.end() ; ++cursor )
                                {
                                        const Method* mem_fn = *cursor ;
                                        if ( mem_fn )
                                                m_methods.push_back(mem_fn->clone()) ;
                                }
                        }

                        virtual ~TestCase()
                        {
                                MethodList::iterator cursor ;
                                for( cursor=m_methods.begin() ; cursor!=m_methods.end() ; ++cursor )
                                {
                                        Method* mem_fn = *cursor ;
                                        delete mem_fn ;
                                }
                                m_methods.clear() ;
                        }

                        template < class T, class TestMethod >
                        void methodAppend(const std::string& name, T& object, const TestMethod& method)
                        {
                                Method* mem_fn = new MethodHolder<T,TestMethod>(name, object, method) ;
                                m_methods.push_back(mem_fn) ;
                        }

                public  :
                        // template method
                        virtual void setupBeforeClass() { } ;
                        virtual void setup() { } ;
                        virtual void tearDown() { } ;
                        virtual void tearDownAfterClass() { } ;

                        // 구동 메소드
                        void run(TestResult& control)
                        {
                                setupBeforeClass() ;

                                MethodList::iterator cursor ;
                                for ( cursor=m_methods.begin() ; cursor!=m_methods.end() ; ++cursor )
                                {
                                        Method* mem_fn = *cursor ;
                                        setup() ;
                                        control.run(*mem_fn) ;
                                        tearDown() ;
                                }
                                tearDownAfterClass() ;
                        }

                        Test* clone() const
                        {
                                return new TestCase(*this) ;
                        }

                } ;




                ///-----------------------------------------------------------------------------------------------------
                /// @brief TestSuite 클래스
                ///-----------------------------------------------------------------------------------------------------
                /// 여러 개의 테스트 또는 테스트 집합들을 묶어내어 하나의 테스트 집합을 만드는 클래스.
                /// Composite 패턴을 이용하여 Tree 자료구조 형태로 테스트들을 묶어낸다.
                ///-----------------------------------------------------------------------------------------------------
                class TestSuite : public Test
                {
                public  :
                        typedef std::vector<Test*> TestList ;
                private :
                        TestList m_tests ;
                public  :
                        /// @brief TestSuite 생성자
                        TestSuite() { }

                        /// @brief TestSuite 복사 생성자
                        TestSuite(const TestSuite& t)
                        {
                                TestList::const_iterator cursor ;
                                for ( cursor = t.m_tests.begin() ; cursor != t.m_tests.end() ; ++cursor )
                                {
                                        const Test* test = *cursor ;
                                        m_tests.push_back(test->clone()) ;
                                }
                        }

                        virtual ~TestSuite()
                        {
                                TestList::iterator cursor ;
                                for ( cursor = m_tests.begin() ; cursor != m_tests.end() ; ++cursor )
                                {
                                        Test* test = *cursor ;
                                        delete test ;
                                }
                                m_tests.clear() ;
                        }

                        void addTest(std::auto_ptr<Test> test)
                        {
                                m_tests.push_back(test->clone()) ;
                                // 파라미터로 넘겨온 test 객체는 자동으로 삭제된다.
                        }

                        int countTestCase() const
                        {
                                int count = 0 ;
                                TestList::const_iterator cursor ;
                                for ( cursor = m_tests.begin() ; cursor != m_tests.end() ; ++cursor )
                                {
                                        const Test* test = *cursor ;
                                        count += test->countTestCase() ;
                                }
                                return count ;
                        }

                        void run(TestResult& control)
                        {
                                TestList::iterator cursor ;
                                for ( cursor = m_tests.begin() ; cursor != m_tests.end() ; ++cursor )
                                {
                                        Test* test = *cursor ;
                                        test->run(control) ;
                                }
                        }

                        Test* clone() const
                        {
                                return new TestSuite(*this) ;
                        }
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief TestRunner 클래스
                ///-----------------------------------------------------------------------------------------------------
                /// 테스트 그룹을 구동하는 클래스.
                ///-----------------------------------------------------------------------------------------------------
                class TestRunner
                {
                public :
                        static void run(std::auto_ptr<Test> test, std::ostream& os=std::cout)
                        {
                                TestResult result ;
                                test->run(result) ;
                                os << result << std::endl ;
                        }
                } ;





                //======================================================================================================
                // ASSERT HELPER 클래스
                // -----------------------------------------------------------------------------------------------------
                // 아래 클래스들은 테스트 메서드 내부에서 사용하는 ASSERT 함수를 Wrapping 한 클래스들로,
                // 사용자 편의성을 위해 실제 사용은 매크로를 이용한다. 삽입시 ASSERT 객체로 전환되어 입력된다.
                //======================================================================================================




                ///-----------------------------------------------------------------------------------------------------
                /// @brief Asserts 인터페이스
                ///-----------------------------------------------------------------------------------------------------
                /// Asserts 클래스의 최상위 객체로 모든 Asserts들은 생성시 Context 정보를 포함해야 한다.
                ///-----------------------------------------------------------------------------------------------------
                class Asserts
                {
                protected :
                        Context m_ctxt ;
                public  :
                        Asserts(const Context& context) : m_ctxt(context)
                        {
                        }
                } ;


                //------------------------------------------------------------------------------------------------------
                // (내부용) Exception 발생 매크로 : 관련 정보들을 Context에 저장 후 throw 한다.
                //------------------------------------------------------------------------------------------------------
                #define THROW_FAILURE_EXCEPTION(msg, info)       \
                        do {                                     \
                                m_ctxt.m_msgs += " "    ;        \
                                m_ctxt.m_msgs += (msg)  ;        \
                                m_ctxt.m_info += (info) ;        \
                                throw FailureException(m_ctxt) ; \
                        }while(0)



                //------------------------------------------------------------------------------------------------------
                // (내부용) Asserts 클래스에 포함된 operator 메서드는 대부분 특정한 패턴을 보이거나 반복되는 구조이기
                // 때문에 Macro로 작성하여 사용한다.
                //------------------------------------------------------------------------------------------------------
                #define OP_P2(_TYPE, _P1, _P2, _COND)                                     \
                        void operator()(_TYPE _P1, _TYPE _P2)                             \
                        {                                                                 \
                                operator()("No Messages", _P1, _P2) ;                     \
                        }                                                                 \
                        void operator()(const std::string& message, _TYPE _P1, _TYPE _P2) \
                        {                                                                 \
                                if ( true == (_COND) ) return ;                           \
                                std::stringstream ss ;                                    \
                                ss << "[" << #_TYPE << "] :: "  ;                         \
                                ss << "[ LHS : " << _P1 << " ] " ;                        \
                                ss << "[ RHS : " << _P2 << " ]"  ;                        \
                                THROW_FAILURE_EXCEPTION((message), ss.str()) ;            \
                        }

                #define TMPL_OP_P2(_P1, _P2, _COND)                                       \
                        template <typename _TYPE>                                         \
                        void operator()(_TYPE _P1, _TYPE _P2)                             \
                        {                                                                 \
                                if ( true == (_COND) ) return ;                           \
                                THROW_FAILURE_EXCEPTION("No Message", "[Object Type]");   \
                        }                                                                 \
                        template <typename _TYPE>                                         \
                        void operator()(const std::string& message, _TYPE _P1, _TYPE _P2) \
                        {                                                                 \
                                if ( true == (_COND) ) return ;                           \
                                THROW_FAILURE_EXCEPTION(message, "[Object Type]");        \
                        }

                // __FUNCTION__ 매크로
                #if defined  (__WIN32)
                        #define __FUNC_NAME__ __FUNCTION__
                #else
                        #define __FUNC_NAME__ __func__
                #endif


                ///-----------------------------------------------------------------------------------------------------
                /// @brief assertEquals Obejct
                ///-----------------------------------------------------------------------------------------------------
                #define assertEquals AssertEquals(__FILE__,__FUNC_NAME__,__LINE__)

                class AssertEquals : public Asserts
                {
                public  :
                        AssertEquals(const char* file, const char* func, int line)
                        : Asserts(Context("[assertEquals]", file, func, "", line))
                        {
                        }

                        TMPL_OP_P2(p1, p2, (p1==p2))
                        OP_P2(int  , p1, p2, (p1==p2))
                        OP_P2(short, p1, p2, (p1==p2))
                        OP_P2(char , p1, p2, (p1==p2))
                        OP_P2(int* , p1, p2, (p1==p2))
                        OP_P2(void*, p1, p2, (p1==p2))
                        OP_P2(const int*  , p1, p2, (p1==p2))
                        OP_P2(const short*, p1, p2, (p1==p2))
                        OP_P2(const std::string&, p1, p2, (p1==p2))

                        // for double

                        void operator()(double p1, double p2, double delta=0.0000001)
                        {
                                operator()("No Messages.", p1, p2, delta) ;
                        }

                        void operator()(const std::string& msg, double p1, double p2, double delta=0.0000001)
                        {
                                double value = p1 - p2 ;
                                if ( (-1*delta) < value && value < delta )
                                        return ;
                                std::stringstream ss ;
                                ss << "[double] :: "  ;
                                ss << "[ LHS : " << p1 << " ] " ;
                                ss << "[ RHS : " << p2 << " ]"  ;
                                THROW_FAILURE_EXCEPTION((msg), ss.str()) ;
                        }
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief Fail Object
                ///-----------------------------------------------------------------------------------------------------
                #define fail AssertFail(__FILE__,__FUNC_NAME__,__LINE__)

                class AssertFail : public Asserts
                {
                public  :
                        AssertFail(const char* file, const char* func, int line)
                        : Asserts(Context("[fail]", file, func, "", line))
                        {
                        }

                        void operator()()
                        {
                                operator()("No Messages.") ;
                        }

                        void operator()(const std::string& msg)
                        {
                                THROW_FAILURE_EXCEPTION(msg, "always shows error-message with [fail] macros.") ;
                        }
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief AssertTrue Object
                ///-----------------------------------------------------------------------------------------------------
                #define assertTrue AssertTrue(__FILE__,__FUNC_NAME__,__LINE__)

                class AssertTrue : public Asserts
                {
                public :
                        AssertTrue(const char* file, const char* func, int line)
                        : Asserts(Context("[assertTrue]",file,func,"", line))
                        {
                        }

                        void operator()(bool task)
                        {
                                operator()("No Messages.", task) ;
                        }

                        void operator()(const std::string msg, bool task)
                        {
                                if ( true == task )
                                        return ;
                                THROW_FAILURE_EXCEPTION(msg, "[Boolean expression] : passes when [true] statement") ;
                        }
                } ;


                ///-----------------------------------------------------------------------------------------------------
                /// @brief AssertFalse Object
                ///-----------------------------------------------------------------------------------------------------
                 #define assertFalse AssertFalse(__FILE__,__FUNC_NAME__,__LINE__)

                class AssertFalse : public Asserts
                {
                public :
                        AssertFalse(const char* file, const char* func, int line)
                        : Asserts(Context("[assertFalse]",file,func,"", line))
                        {
                        }

                        void operator()(bool task)
                        {
                                operator()("No Messages.", task) ;
                        }

                        void operator()(const std::string msg, bool task)
                        {
                                if ( false == task )
                                        return ;
                                THROW_FAILURE_EXCEPTION(msg, "[Boolean expression] : passes when [false] statement") ;
                        }
                } ;






                ///-----------------------------------------------------------------------------------------------------
                /// @brief TestResult를 ostream 객체에 출력하는 간단한 Helper 함수
                ///-----------------------------------------------------------------------------------------------------
                inline std::ostream& operator<<(std::ostream& os, const TestResult& test)
                {
                        os << "\n" ;
                        os << "================================================================================\n" ;

                        if ( true == test.wasSuccessful() )
                                os << "Result : Succeed\n" ;
                        else
                                os << "Result : Failed\n" ;

                        os << "--------------------------------------------------------------------------------\n" ;
                        os << "- Total Test Count : " << test.totalRunCount()<< "\n" ;
                        os << "   - Success Count : " << test.successCount() << "\n" ;
                        os << "   - Failure Count : " << test.failureCount() << "\n" ;
                        os << "   - Error   Count : " << test.errorCount()   << "\n" ;

                        os << "   - Success Rates : " ;
                        if ( true == test.wasSuccessful() )
                                os << "100 %\n" ;
                        else if ( 0 == test.totalRunCount() )
                                os << "N/A\n" ;
                        else
                        {
                                double rate ;
                                rate = static_cast<double>(test.successCount()) ;
                                rate /= test.totalRunCount() ;
                                rate *= 100 ;
                                os << rate << " %\n" ;
                        }

                        os << "   - Total   Times : " << test.m_timer.records() << " seconds \n" ;
                        os << "--------------------------------------------------------------------------------\n" ;

                        if ( false == test.wasSuccessful() )
                        {
                                if ( false == test.m_failure.empty() )
                                {
                                        os << "\n\n" ;
                                        os << "================================================================================\n" ;
                                        os << "Failure Info (" << test.failureCount() << "/" << test.totalRunCount() << ")\n" ;
                                        os << "--------------------------------------------------------------------------------\n" ;

                                        std::vector<Context>::const_iterator iter ;
                                        int idx = 0 ;
                                        for ( iter=test.m_failure.begin() ; iter!=test.m_failure.end() ; ++iter)
                                        {
                                                const Context& info = *iter ;

                                                os << "  [" << std::setw(3) << ++idx << "] " ;
                                                os << "code : " << info.m_func << "() in " << info.m_file << " (" << info.m_line << ")\n" ;
                                                os << "       " << "msgs : " << info.m_msgs << "\n" ;
                                                os << "       " << "info : " << info.m_info << "\n\n" ;
                                        }
                                        os << "--------------------------------------------------------------------------------\n" ;
                                }

                                if ( false == test.m_error.empty() )
                                {
                                        os << "\n\n" ;
                                        os << "================================================================================\n" ;
                                        os << "Error Info (" << test.errorCount() << "/" << test.totalRunCount() << ")\n" ;
                                        os << "--------------------------------------------------------------------------------\n" ;

                                        std::vector<Context>::const_iterator iter ;
                                        int idx = 0 ;
                                        for ( iter=test.m_error.begin() ; iter!=test.m_error.end() ; ++iter)
                                        {
                                                const Context& info = *iter ;

                                                os << "  [" << std::setw(3) << ++idx << "] " ;
                                                os << "code : " << info.m_func << "() in " << info.m_file << " (" << info.m_line << ")\n" ;
                                                os << "       " << "msgs : " << info.m_msgs << "\n" ;
                                                os << "       " << "info : " << info.m_info << "\n\n" ;
                                        }
                                        os << "--------------------------------------------------------------------------------\n" ;
                                }
                        }
                        else
                        {
                                os << "\n\n" ;
                                os << "================================================================================\n" ;
                                os << "Success Info (" << test.successCount() << "/" << test.totalRunCount() << ")\n" ;
                                os << "--------------------------------------------------------------------------------\n" ;

                                std::vector<Context>::const_iterator iter ;
                                int idx = 0 ;
                                for ( iter=test.m_success.begin() ; iter!=test.m_success.end() ; ++iter)
                                {
                                        const Context& info = *iter ;
                                        os << "  [" << std::setw(3) << ++idx << "] " ;
                                        os << "function : " << info.m_func << "\n"  ;
                                }
                                os << "--------------------------------------------------------------------------------\n" ;


                        }
                        os << std::endl ;
                        return os ;
                }



        } // namespace cppunit
} // namespace jss
#endif // __JSS_CPPUNIT_H__
