/**
   @file
   @author Shin'ichiro NAKAOKA
*/

#ifndef CNOID_CHOREOGRAPHY_POSE_SEQ_INTERPOLATOR_H_INCLUDED
#define CNOID_CHOREOGRAPHY_POSE_SEQ_INTERPOLATOR_H_INCLUDED

#include "PoseSeq.h"
#include <cnoid/PoseProvider>
#include <cnoid/SignalProxy>
#include <boost/shared_ptr.hpp>
#include "exportdecl.h"

namespace cnoid {

    class YamlMapping;
    
    class PSIImpl;
        
    class CNOID_EXPORT PoseSeqInterpolator : public PoseProvider
    {
      public:

        PoseSeqInterpolator();

        void setBody(BodyPtr body);
        Body* body() const;

        void setLinearInterpolationJoint(int jointId);

        void addFootLink(int linkIndex, const Vector3& soleCenter);

        void setLipSyncShapes(const YamlMapping& info);
        const std::vector<int>& lipSyncLinkIndices();
            
        void setPoseSeq(PoseSeqPtr seq);

        void setTimeScaleRatio(double ratio);

        double beginningTime() const;
        double endingTime() const;
            
        void enableStealthyStepMode(bool on);
        void setStealthyStepParameters(
            double heightRatioThresh,
            double flatLiftingHeight, double flatLandingHeight,
            double impactReductionHeight, double impactReductionTime);

        void enableAutoZmpAdjustmentMode(bool on);
        void setZmpAdjustmentParameters(
            double minTransitionTime, double centeringTimeThresh, double timeMarginBeforeLifting);

        void enableLipSyncMix(bool on);

        /**
           This function has not been implemented yet.
        */
        void setAutoUpdateMode(bool on);
            
        bool update();

        SignalProxy< boost::signal<void()> > sigUpdated();
            
        bool interpolate(double time);
        bool interpolate(double time, int waistLinkIndex, const Vector3& waistTranslation);

        virtual bool seek(double time);
        virtual bool seek(double time, int waistLinkIndex, const Vector3& waistTranslation);
            
        /**
           @return -1 if base link is not set for the time segment
        */
        int baseLinkIndex() const;
        bool getBaseLinkPosition(Vector3& out_p, Matrix3& out_R) const;

        boost::optional<double> jointPosition(int jointId) const;
        boost::optional<Vector3> zmp() const;

        virtual void getJointPositions(std::vector< boost::optional<double> >& out_q) const;

      private:

        PSIImpl* impl;
    };

    typedef boost::shared_ptr<PoseSeqInterpolator> PoseSeqInterpolatorPtr;
}

#endif
