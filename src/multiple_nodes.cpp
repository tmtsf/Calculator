#include "node.hpp"

namespace Calculator
{
  namespace
  {
    struct MultipleNode : public ASTNode
    {
      MultipleNode( const node_ptr_t& child,
                    const std::string& description,
                    const std::string& inverse ) :
        m_Desc( description ),
        m_Inverse( inverse )
      {
        addChildNode( child, true );
      }
      virtual ~MultipleNode( void )
      { }
    public:
      virtual double calculate( void ) const = 0;
    public:
      virtual const std::string& description( void ) const
      {
        return m_Desc;
      }
      virtual void addChildNode( const node_ptr_t& newChild,
                                 bool weight )
      {
        m_ChildNodes.push_back( newChild );
        m_Weights.push_back( weight );
      }
      virtual void print( int indent ) const
      {
        std::cout << std::endl;
        for (int i=0; i<indent; ++i)
          std::cout << ' ';
        std::cout << description() << ": ";

        auto it = m_ChildNodes.cbegin();
        auto jt = m_Weights.cbegin();
        auto itEnd = m_ChildNodes.cend();
        for ( ; it != itEnd; ++it, ++jt )
        {
          (*it)->print( indent + 2 );
          if ( ! *jt )
            std::cout << "    " << m_Inverse;
        }
      }
    protected:
      node_ptr_coll_t m_ChildNodes;
      boolean_vec_t m_Weights;
    private:
      std::string m_Desc;
      std::string m_Inverse;
    };

    struct MultipleSummationNode : public MultipleNode
    {
      MultipleSummationNode( const node_ptr_t& child ) :
        MultipleNode( child, "MultipleSum", "Negation" )
      { }
      virtual ~MultipleSummationNode( void )
      { }
    public:
      virtual double calculate( void ) const override
      {
        double result = 0.0;
        auto it = m_ChildNodes.cbegin();
        auto itEnd = m_ChildNodes.cend();
        auto jt = m_Weights.cbegin();
        for ( ; it != itEnd; ++it, ++jt )
        {
          bool weight = *jt;
          double thisValue = (*it)->calculate();
          result += weight ? thisValue : -thisValue;
        }
        return result;
      }
    };

    struct MultipleMultiplicationNode : public MultipleNode
    {
      MultipleMultiplicationNode( const node_ptr_t& child ) :
        MultipleNode( child, "MultipleProduct", "Inversion" )
      { }
      virtual ~MultipleMultiplicationNode( void )
      { }
    public:
      virtual double calculate( void ) const override
      {
        double result = 1.0;
        auto it = m_ChildNodes.cbegin();
        auto itEnd = m_ChildNodes.cend();
        auto jt = m_Weights.cbegin();
        for ( ; it != itEnd; ++it, ++jt )
        {
          bool weight = *jt;
          double thisValue = (*it)->calculate();
          result *= weight ? thisValue : 1. / thisValue;
        }
        return result;
      }
    };
  }

  node_ptr_t ASTNode::formMultipleSummationNode( const node_ptr_t& child )
  {
    return std::make_shared<MultipleSummationNode>( child );
  }

  node_ptr_t ASTNode::formMultipleMultiplicationNode( const node_ptr_t& child )
  {
    return std::make_shared<MultipleMultiplicationNode>( child );
  }
}