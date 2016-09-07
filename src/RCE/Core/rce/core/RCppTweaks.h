#ifndef RCE_CORE_RCPPTWEAKS_H
#define RCE_CORE_RCPPTWEAKS_H


namespace rce {
    namespace core {

        template <typename T> int sgn(T val) {
            return (T(0) < val) - (val < T(0));
        }

        class RCppTweaks
        {
        public:
//            RCppTweaks();
//            ~RCppTweaks();
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RCPPTWEAKS_H
