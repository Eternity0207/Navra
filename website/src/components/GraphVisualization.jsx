import React, { useRef, useEffect, useState } from 'react';
import ForceGraph2D from 'react-force-graph-2d';

const GraphVisualization = ({ routePath = [], graphData }) => {
  const fgRef = useRef();
  const [highlightNodes, setHighlightNodes] = useState(new Set());
  const [highlightLinks, setHighlightLinks] = useState(new Set());
  const [hoverNode, setHoverNode] = useState(null);
  const [isDarkMode, setIsDarkMode] = useState(true);

  // Detect theme changes
  useEffect(() => {
    const checkTheme = () => {
      const htmlElement = document.documentElement;
      const theme = htmlElement.getAttribute('data-theme');
      setIsDarkMode(theme !== 'light');
    };

    checkTheme();
    
    const observer = new MutationObserver(checkTheme);
    observer.observe(document.documentElement, {
      attributes: true,
      attributeFilter: ['data-theme']
    });

    return () => observer.disconnect();
  }, []);

  // Theme-aware colors
  const colors = isDarkMode ? {
    background: '#0f1419',
    nodeDefault: '#94a3b8',
    nodeHighlight: '#6366f1',
    nodeHighlightBorder: '#4f46e5',
    nodeGlow: 'rgba(99, 102, 241, 0.4)',
    linkDefault: '#374151',
    linkHighlight: '#6366f1',
    labelDefault: '#94a3b8',
    labelHighlight: '#f1f5f9',
    badgeGradient: { start: '#34d399', end: '#10b981' },
    timeLabel: { bg: '#1f2937', text: '#6366f1', border: '#4f46e5' },
    particle: '#6366f1',
  } : {
    background: '#f8fafc',
    nodeDefault: '#64748b',
    nodeHighlight: '#6366f1',
    nodeHighlightBorder: '#4f46e5',
    nodeGlow: 'rgba(99, 102, 241, 0.3)',
    linkDefault: '#cbd5e1',
    linkHighlight: '#6366f1',
    labelDefault: '#475569',
    labelHighlight: '#1e293b',
    badgeGradient: { start: '#34d399', end: '#10b981' },
    timeLabel: { bg: '#ffffff', text: '#6366f1', border: '#6366f1' },
    particle: '#6366f1',
  };

  useEffect(() => {
    if (routePath && routePath.length > 0 && graphData) {
      const highlightNodesSet = new Set(routePath);
      const highlightLinksSet = new Set();

      // Find links that connect consecutive nodes in the route
      for (let i = 0; i < routePath.length - 1; i++) {
        const sourceNode = routePath[i];
        const targetNode = routePath[i + 1];

        // Find the link connecting these two nodes
        const link = graphData.links.find(l => {
          const source = typeof l.source === 'object' ? l.source.id : l.source;
          const target = typeof l.target === 'object' ? l.target.id : l.target;
          
          return (source === sourceNode && target === targetNode) ||
                 (source === targetNode && target === sourceNode);
        });

        if (link) {
          highlightLinksSet.add(link);
        }
      }

      setHighlightNodes(highlightNodesSet);
      setHighlightLinks(highlightLinksSet);

      // Zoom to fit the highlighted path
      if (fgRef.current && highlightNodesSet.size > 0) {
        setTimeout(() => {
          fgRef.current.zoomToFit(400, 80);
        }, 200);
      }
    } else {
      setHighlightNodes(new Set());
      setHighlightLinks(new Set());
    }
  }, [routePath, graphData]);

  const paintNode = (node, ctx, globalScale) => {
    const label = node.name || node.id;
    const fontSize = 14 / globalScale;
    const isHighlighted = highlightNodes.has(node.id);
    const isHovered = hoverNode === node;
    const nodeSize = isHighlighted ? 8 : 5;

    ctx.save();

    // Outer glow for highlighted nodes
    if (isHighlighted) {
      ctx.beginPath();
      ctx.arc(node.x, node.y, nodeSize + 4, 0, 2 * Math.PI);
      const gradient = ctx.createRadialGradient(node.x, node.y, nodeSize, node.x, node.y, nodeSize + 4);
      gradient.addColorStop(0, colors.nodeGlow);
      gradient.addColorStop(1, 'transparent');
      ctx.fillStyle = gradient;
      ctx.fill();
    }

    // Node circle with gradient
    ctx.beginPath();
    ctx.arc(node.x, node.y, nodeSize, 0, 2 * Math.PI);

    if (isHighlighted) {
      const gradient = ctx.createRadialGradient(
        node.x - nodeSize / 3, node.y - nodeSize / 3, 0,
        node.x, node.y, nodeSize
      );
      gradient.addColorStop(0, '#818cf8');
      gradient.addColorStop(1, colors.nodeHighlight);
      ctx.fillStyle = gradient;
    } else {
      ctx.fillStyle = isHovered ? colors.labelDefault : colors.nodeDefault;
    }
    ctx.fill();

    // Node border
    ctx.strokeStyle = isHighlighted ? colors.nodeHighlightBorder : (isDarkMode ? '#4b5563' : '#cbd5e1');
    ctx.lineWidth = isHighlighted ? 2.5 / globalScale : 1.5 / globalScale;
    ctx.stroke();

    // Shadow for depth
    if (isHighlighted || isHovered) {
      ctx.shadowColor = isDarkMode ? 'rgba(0, 0, 0, 0.5)' : 'rgba(0, 0, 0, 0.15)';
      ctx.shadowBlur = 8 / globalScale;
      ctx.shadowOffsetY = 2 / globalScale;
    }

    // Label with better styling
    ctx.font = `${isHighlighted ? 'bold' : '600'} ${fontSize}px -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';

    // Label shadow for better readability
    if (isDarkMode) {
      ctx.shadowColor = 'rgba(0, 0, 0, 0.8)';
      ctx.shadowBlur = 4 / globalScale;
    } else {
      ctx.shadowColor = 'rgba(255, 255, 255, 0.9)';
      ctx.shadowBlur = 3 / globalScale;
    }

    ctx.fillStyle = isHighlighted ? colors.labelHighlight : colors.labelDefault;
    ctx.fillText(label, node.x, node.y - nodeSize - 10);

    ctx.shadowColor = 'transparent';
    ctx.shadowBlur = 0;

    // Order badge with enhanced styling
    if (isHighlighted && routePath.includes(node.id)) {
      const index = routePath.indexOf(node.id) + 1;
      const badgeSize = 10;

      // Badge shadow
      ctx.shadowColor = isDarkMode ? 'rgba(0, 0, 0, 0.4)' : 'rgba(0, 0, 0, 0.2)';
      ctx.shadowBlur = 4 / globalScale;
      ctx.shadowOffsetY = 1 / globalScale;

      // Badge circle with gradient
      ctx.beginPath();
      ctx.arc(node.x + nodeSize + 2, node.y - nodeSize - 2, badgeSize, 0, 2 * Math.PI);
      const badgeGradient = ctx.createRadialGradient(
        node.x + nodeSize + 2, node.y - nodeSize - 2, 0,
        node.x + nodeSize + 2, node.y - nodeSize - 2, badgeSize
      );
      badgeGradient.addColorStop(0, colors.badgeGradient.start);
      badgeGradient.addColorStop(1, colors.badgeGradient.end);
      ctx.fillStyle = badgeGradient;
      ctx.fill();

      // Badge border
      ctx.strokeStyle = '#fff';
      ctx.lineWidth = 2 / globalScale;
      ctx.stroke();

      ctx.shadowColor = 'transparent';
      ctx.shadowBlur = 0;

      // Badge number
      ctx.fillStyle = '#fff';
      ctx.font = `bold ${11 / globalScale}px -apple-system, BlinkMacSystemFont, sans-serif`;
      ctx.fillText(index.toString(), node.x + nodeSize + 2, node.y - nodeSize - 2);
    }

    ctx.restore();
  };

  const paintLink = (link, ctx, globalScale) => {
    const isHighlighted = highlightLinks.has(link);
    
    const sourceX = typeof link.source === 'object' ? link.source.x : 0;
    const sourceY = typeof link.source === 'object' ? link.source.y : 0;
    const targetX = typeof link.target === 'object' ? link.target.x : 0;
    const targetY = typeof link.target === 'object' ? link.target.y : 0;

    ctx.save();

    // Draw link with gradient if highlighted
    ctx.beginPath();
    ctx.moveTo(sourceX, sourceY);
    ctx.lineTo(targetX, targetY);

    if (isHighlighted) {
      const gradient = ctx.createLinearGradient(sourceX, sourceY, targetX, targetY);
      gradient.addColorStop(0, '#818cf8');
      gradient.addColorStop(0.5, colors.linkHighlight);
      gradient.addColorStop(1, '#4f46e5');
      ctx.strokeStyle = gradient;
      ctx.lineWidth = 4 / globalScale;
      ctx.lineCap = 'round';
    } else {
      ctx.strokeStyle = colors.linkDefault;
      ctx.lineWidth = 1.5 / globalScale;
    }
    ctx.stroke();

    // Animated arrow for highlighted links
    if (isHighlighted) {
      const arrowLength = 10 / globalScale;
      const arrowWidth = 6 / globalScale;

      const dx = targetX - sourceX;
      const dy = targetY - sourceY;
      const angle = Math.atan2(dy, dx);

      const midX = (sourceX + targetX) / 2;
      const midY = (sourceY + targetY) / 2;

      ctx.save();
      ctx.translate(midX, midY);
      ctx.rotate(angle);

      ctx.beginPath();
      ctx.moveTo(0, 0);
      ctx.lineTo(-arrowLength, arrowWidth);
      ctx.lineTo(-arrowLength, -arrowWidth);
      ctx.closePath();

      const arrowGradient = ctx.createLinearGradient(-arrowLength, 0, 0, 0);
      arrowGradient.addColorStop(0, '#4f46e5');
      arrowGradient.addColorStop(1, colors.linkHighlight);
      ctx.fillStyle = arrowGradient;
      ctx.fill();

      ctx.restore();
    }

    // Enhanced time label with pill background
    if (isHighlighted && link.value) {
      const label = `${link.value} min`;
      const midX = (sourceX + targetX) / 2;
      const midY = (sourceY + targetY) / 2;

      ctx.font = `bold ${11 / globalScale}px -apple-system, BlinkMacSystemFont, sans-serif`;
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';

      const textWidth = ctx.measureText(label).width;
      const padding = 6 / globalScale;
      const height = 18 / globalScale;

      // Pill shadow
      ctx.shadowColor = isDarkMode ? 'rgba(0, 0, 0, 0.3)' : 'rgba(0, 0, 0, 0.12)';
      ctx.shadowBlur = 4 / globalScale;
      ctx.shadowOffsetY = 2 / globalScale;

      // Pill background
      ctx.beginPath();
      ctx.roundRect(
        midX - textWidth / 2 - padding,
        midY - height / 2,
        textWidth + padding * 2,
        height,
        height / 2
      );
      ctx.fillStyle = colors.timeLabel.bg;
      ctx.fill();

      // Pill border
      ctx.strokeStyle = colors.timeLabel.border;
      ctx.lineWidth = 1.5 / globalScale;
      ctx.stroke();

      ctx.shadowColor = 'transparent';
      ctx.shadowBlur = 0;

      // Text
      ctx.fillStyle = colors.timeLabel.text;
      ctx.fillText(label, midX, midY);
    }

    ctx.restore();
  };

  if (!graphData || !graphData.nodes || !graphData.links) {
    return (
      <div style={{ 
        width: '100%',
        height: '100%',
        display: 'flex', 
        alignItems: 'center', 
        justifyContent: 'center',
        background: colors.background 
      }}>
        <p style={{ color: colors.labelDefault }}>Loading graph...</p>
      </div>
    );
  }

  return (
    <ForceGraph2D
      ref={fgRef}
      graphData={graphData}
      width={window.innerWidth}
      height={window.innerHeight}
      backgroundColor={colors.background}
      nodeCanvasObject={paintNode}
      nodeCanvasObjectMode={() => 'replace'}
      linkCanvasObject={paintLink}
      linkCanvasObjectMode={() => 'replace'}
      linkDirectionalParticles={link => highlightLinks.has(link) ? 4 : 0}
      linkDirectionalParticleWidth={3}
      linkDirectionalParticleSpeed={0.005}
      linkDirectionalParticleColor={() => colors.particle}
      onNodeHover={setHoverNode}
      d3VelocityDecay={0.3}
      d3AlphaDecay={0.02}
      cooldownTime={2000}
      warmupTicks={50}
      enableNodeDrag={true}
      enableZoomInteraction={true}
      enablePanInteraction={true}
    />
  );
};

export default GraphVisualization;
