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
                /// �׽�Ʈ ����� �ɸ� �ð��� �����ϱ� ���� ���Ǵ� ������ Util Ŭ����.
                /// start(), stop()�� �ݺ������� ȣ���� �� ������ ��ü �ð��� ��� �����ȴ�. �ٽ� �ʱ�ȭ�� �ϰ���
                /// �Ѵٸ� clear() �Լ��� ����Ͽ��� �Ѵ�. �� ���� �ð��� records()�Լ��� ������� �� �����̴�.
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
                /// �ڵ��� ��ġ ������ ��� �ִ� Ŭ����
                /// POD(Plain Old Data) Ÿ���� ������ Ŭ����. �ڵ� ���ݿ��� ���ȴ�.
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
                /// @brief FailureException Ŭ����
                ///-----------------------------------------------------------------------------------------------------
                /// ���� �׽�Ʈ���� ���Ǵ� assert �迭 �Լ��� �����ϸ� ���ܰ� �߻��ϴµ�, �� �� �߻��Ǵ� ���� ��ü��
                /// FailureException ��ü�̴�. ���н� �ش� ��ġ�� �������� �Բ� �����Ͽ� ���ܷ� ��ȯ�ǰ� �ȴ�.
                ///-----------------------------------------------------------------------------------------------------
                class FailureException : public std::exception
                {
                private :
                        Context m_context ; // ��ġ ����
                public  :
                        /// @brief ������
                        FailureException() : m_context()
                        {
                        }
                        /// @brief ���� ������
                        FailureException(const Context& context) : m_context(context)
                        {
                        }
                        /// @brief �Ҹ���
                        ~FailureException() throw()
                        {
                        }
                        /// @brief ���� �޼��� ��ȯ
                        const char* what() const throw()
                        {
                                return m_context.m_msgs.c_str() ;
                        }
                        /// @brief ���� ������ Context ������ ��ȯ�Ѵ�.
                        /// Context���� �ڵ� ��ġ �� ���� �޽����� �����ϰ� �ִ�.
                        const Context& context() const throw()
                        {
                                return m_context ;
                        }
                } ;






                ///-----------------------------------------------------------------------------------------------------
                /// @brief Method-Object Interface
                ///-----------------------------------------------------------------------------------------------------
                /// ����ڰ� ���� �׽�Ʈ�� ����� �޼������ ����ϰ� �Ǵµ�, �̸� ���ο����� �ϳ��� ��ü ������
                /// �����Ͽ� ����ϰ� �ȴ�. �̸� ���� ����ϴ� Method ��ü�� �������̽��� ���� ���ÿ��� template
                /// ������� �����Ǿ� ���ǰ� �ȴ�. Concrete ��ü�� ������ �����ϵ��� �Ѵ�.
                ///-----------------------------------------------------------------------------------------------------
                class Method
                {
                public  :
                        virtual ~Method() { }
                        virtual void execute() const = 0 ;     // ��ϵ� �޼ҵ带 �����Ѵ�.
                        virtual Method* clone() const = 0 ;    // ���� ��ü�� �����Ѵ�.(deep copy)
                        virtual const char* name() const = 0 ; // ���� ��ϵ� �޼ҵ��� �̸��� ��ȯ�Ѵ�.
                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief Method Class Holder
                ///-----------------------------------------------------------------------------------------------------
                /// �Լ� ȣ���� �������ִ� Ŭ������ ���� ��ü�� �� ��ü�� ������ �ִ� �޼��带 ���ø� �Ķ���ͷ�
                /// �Է¹޾� ȣ���� ���ִ� Ŭ�����̴�. ������ Command ���� ��ü�� �����ϸ� �ȴ�.
                /// �̸� ���� �޼����� ȣ�� ������ �� Ŭ������ ������ �� �ִ�. ( Command ������ ���� )
                /// ���ø��� ����ϴ� ������ Test Framework ������ ��� �������� � Ŭ������ � �޼��带 �������
                /// �Ǵ��ϱ� ��Ʊ� �����̴�.
                ///-----------------------------------------------------------------------------------------------------
                template < class T, class TestMethod >
                class MethodHolder : public Method
                {
                private :
                        std::string m_name   ; // �׽�Ʈ �޼��� �̸� ( �޽��� ����� ���� ��� )
                        T&          m_object ; // �׽�Ʈ �޼��带 ������ ���� ��ü
                        TestMethod  m_method ; // �׽�Ʈ �޼��带 Wrapping �� �޼��� ��ü

                public  :
                        /// @brief MethodHolder ������
                        MethodHolder(const std::string& name, T& object, const TestMethod& testMethod)
                        : m_name(name), m_object(object), m_method(testMethod)
                        {
                        }

                        /// @brief MethodHolder ������
                        MethodHolder(T& object, const TestMethod& testMethod)
                        : m_name(""), m_object(object), m_method(testMethod)
                        {
                        }

                        /// @brief MethodHolder ���� ������
                        MethodHolder(const MethodHolder& holder)
                        : m_name(holder.m_name), m_object(holder.m_object), m_method(holder.m_method)
                        {
                        }

                        /// @brief ���� �Լ��� ȣ���ϴ� �޼���.
                        /// Command ������ execute() ������ �Ѵ�.
                        void execute() const
                        {
                                (m_object.*m_method)() ;
                        }

                        /// @brief �Լ� �̸��� ��ȯ�ϴ� �޼���
                        const char* name() const throw()
                        {
                                return m_name.c_str() ;
                        }

                        /// @brief ���� �޼���
                        Method* clone() const
                        {
                                return new MethodHolder(*this) ;
                        }

                } ;



                ///-----------------------------------------------------------------------------------------------------
                /// @brief TestResult Ŭ����
                ///-----------------------------------------------------------------------------------------------------
                /// �׽�Ʈ�� ����� ��Ƴ��� Ŭ���� UnitTest ���� ����ϴ� TestResult�� �ϴ� ������ ����.
                /// �׽�Ʈ ����� ������Ű��, �׿� ���� �׽�Ʈ ������� �����Ѵ�.
                /// ���� Test ��ü���� �Ѱܹ޾� ����(Trigger)��Ű�� ����� �� Ŭ������ ���ԵǾ� �ִ�.
                ///-----------------------------------------------------------------------------------------------------
                class TestResult
                {
                private :
                        Timer m_timer ; // �ð� ������ ��ü

                        std::vector<Context> m_success ; // ������ �׽�Ʈ ���� ��ü
                        std::vector<Context> m_failure ; // ������ �׽�Ʈ ���� ��ü
                        std::vector<Context> m_error   ; // ���� �׽�Ʈ ���� ��ü
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

                        /// @brief ���� �׽�Ʈ�� ��� �������̾������� üũ�ϴ� �޼���
                        bool wasSuccessful() const
                        {
                                if ( 0 == errorCount() && 0 == failureCount() )
                                        return true ;
                                return false ;
                        }

                        /// @brief �׽�Ʈ ��� �ʱ�ȭ
                        void clear()
                        {
                                m_timer.clear()   ;
                                m_success.clear() ;
                                m_failure.clear() ;
                                m_error.clear()   ;
                        }

                        /// @brief �׽�Ʈ ���� �Լ�. �ϳ��� �޼��忡 ���ؼ� ������ �ϰ� �ȴ�.
                        bool run(const Method& method)
                        {
                                m_timer.start() ;
                                try
                                {
                                        method.execute() ; // �Ѱܹ��� �׽�Ʈ �Լ��� ȣ���Ѵ�.

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

                        // �׽�Ʈ ����� ostream �� ������ִ� Helper �Լ�
                        friend std::ostream& operator<<(std::ostream& os, const TestResult& test) ;
                } ;




                ///-----------------------------------------------------------------------------------------------------
                /// @brief Test �������̽�
                ///-----------------------------------------------------------------------------------------------------
                /// ����ڰ� ����� �׽�Ʈ ��ü�� �������̽�
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
                /// @brief TestCase Ŭ����
                ///-----------------------------------------------------------------------------------------------------
                /// ����ڰ� �ۼ��� �ϳ��� �׽�Ʈ�� �ǹ��Ѵ�.
                /// UnitTest �����ӿ�ũ�� TestCase �� �� ����� �����ϴ�. �׷��� C++������ ���÷���(reflection) �����
                /// ��� Level ���� �������� �ʱ� ������ ������ �ణ �ٸ���. ���⼭�� ��ũ�θ� ����Ѵ�.
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

                        // ���� �޼ҵ�
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
                /// @brief TestSuite Ŭ����
                ///-----------------------------------------------------------------------------------------------------
                /// ���� ���� �׽�Ʈ �Ǵ� �׽�Ʈ ���յ��� ����� �ϳ��� �׽�Ʈ ������ ����� Ŭ����.
                /// Composite ������ �̿��Ͽ� Tree �ڷᱸ�� ���·� �׽�Ʈ���� �����.
                ///-----------------------------------------------------------------------------------------------------
                class TestSuite : public Test
                {
                public  :
                        typedef std::vector<Test*> TestList ;
                private :
                        TestList m_tests ;
                public  :
                        /// @brief TestSuite ������
                        TestSuite() { }

                        /// @brief TestSuite ���� ������
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
                                // �Ķ���ͷ� �Ѱܿ� test ��ü�� �ڵ����� �����ȴ�.
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
                /// @brief TestRunner Ŭ����
                ///-----------------------------------------------------------------------------------------------------
                /// �׽�Ʈ �׷��� �����ϴ� Ŭ����.
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
                // ASSERT HELPER Ŭ����
                // -----------------------------------------------------------------------------------------------------
                // �Ʒ� Ŭ�������� �׽�Ʈ �޼��� ���ο��� ����ϴ� ASSERT �Լ��� Wrapping �� Ŭ�������,
                // ����� ���Ǽ��� ���� ���� ����� ��ũ�θ� �̿��Ѵ�. ���Խ� ASSERT ��ü�� ��ȯ�Ǿ� �Էµȴ�.
                //======================================================================================================




                ///-----------------------------------------------------------------------------------------------------
                /// @brief Asserts �������̽�
                ///-----------------------------------------------------------------------------------------------------
                /// Asserts Ŭ������ �ֻ��� ��ü�� ��� Asserts���� ������ Context ������ �����ؾ� �Ѵ�.
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
                // (���ο�) Exception �߻� ��ũ�� : ���� �������� Context�� ���� �� throw �Ѵ�.
                //------------------------------------------------------------------------------------------------------
                #define THROW_FAILURE_EXCEPTION(msg, info)       \
                        do {                                     \
                                m_ctxt.m_msgs += " "    ;        \
                                m_ctxt.m_msgs += (msg)  ;        \
                                m_ctxt.m_info += (info) ;        \
                                throw FailureException(m_ctxt) ; \
                        }while(0)



                //------------------------------------------------------------------------------------------------------
                // (���ο�) Asserts Ŭ������ ���Ե� operator �޼���� ��κ� Ư���� ������ ���̰ų� �ݺ��Ǵ� �����̱�
                // ������ Macro�� �ۼ��Ͽ� ����Ѵ�.
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

                // __FUNCTION__ ��ũ��
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
                /// @brief TestResult�� ostream ��ü�� ����ϴ� ������ Helper �Լ�
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
