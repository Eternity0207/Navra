import React, { useRef, useEffect, useState } from 'react';
import ForceGraph2D from 'react-force-graph-2d';

const GraphVisualization = ({ routePath = [], edges }) => {
  const fgRef = useRef();
  const [graphData, setGraphData] = useState({ nodes: [], links: [] });
  const [highlightNodes, setHighlightNodes] = useState(new Set());
  const [highlightLinks, setHighlightLinks] = useState(new Set());
  const [hoverNode, setHoverNode] = useState(null);
  const [isDarkMode, setIsDarkMode] = useState(
    window.matchMedia('(prefers-color-scheme: dark)').matches
  );

  // Detect theme changes
  useEffect(() => {
    const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
    const handleChange = (e) => setIsDarkMode(e.matches);
    mediaQuery.addEventListener('change', handleChange);
    return () => mediaQuery.removeEventListener('change', handleChange);
  }, []);

  // Theme-aware colors
  const colors = isDarkMode ? {
    // Dark Mode
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
    // Light Mode
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
    const nodeSet = new Set();
    const links = edges.map((edge) => {
      const [from, to, time] = edge.split(',');
      nodeSet.add(from.trim());
      nodeSet.add(to.trim());
      return {
        source: from.trim(),
        target: to.trim(),
        value: parseInt(time.trim()),
      };
    });

    const nodes = Array.from(nodeSet).map((name) => ({
      id: name,
      name: name,
      val: 12,
    }));

    setGraphData({ nodes, links });
  }, [edges]);

  useEffect(() => {
    if (routePath && routePath.length > 0) {
      const highlightNodesSet = new Set(routePath);
      const highlightLinksSet = new Set();

      for (let i = 0; i < routePath.length - 1; i++) {
        const link = graphData.links.find(
          (l) =>
            (l.source.id || l.source) === routePath[i] &&
            (l.target.id || l.target) === routePath[i + 1]
        );
        if (link) highlightLinksSet.add(link);
      }

      setHighlightNodes(highlightNodesSet);
      setHighlightLinks(highlightLinksSet);

      if (fgRef.current) {
        setTimeout(() => fgRef.current.zoomToFit(400, 100), 100);
      }
    } else {
      setHighlightNodes(new Set());
      setHighlightLinks(new Set());
    }
  }, [routePath, graphData]);

  const paintNode = (node, ctx, globalScale) => {
    const label = node.name;
    const fontSize = 14 / globalScale;
    const isHighlighted = highlightNodes.has(node.id);
    const isHovered = hoverNode === node;
    const nodeSize = isHighlighted ? 8 : 5;

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
  };

  const paintLink = (link, ctx, globalScale) => {
    const isHighlighted = highlightLinks.has(link);

    // Draw link with gradient if highlighted
    ctx.beginPath();
    ctx.moveTo(link.source.x, link.source.y);
    ctx.lineTo(link.target.x, link.target.y);

    if (isHighlighted) {
      const gradient = ctx.createLinearGradient(
        link.source.x, link.source.y,
        link.target.x, link.target.y
      );
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

      const dx = link.target.x - link.source.x;
      const dy = link.target.y - link.source.y;
      const angle = Math.atan2(dy, dx);

      const midX = (link.source.x + link.target.x) / 2;
      const midY = (link.source.y + link.target.y) / 2;

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
    if (isHighlighted) {
      const label = `${link.value} min`;
      const midX = (link.source.x + link.target.x) / 2;
      const midY = (link.source.y + link.target.y) / 2;

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
  };

  return (
    <div className="graph-wrapper">
      <ForceGraph2D
        ref={fgRef}
        graphData={graphData}
        backgroundColor={colors.background}
        nodeCanvasObject={paintNode}
        nodeCanvasObjectMode={() => 'replace'}
        linkCanvasObject={paintLink}
        linkCanvasObjectMode={() => 'replace'}
        linkDirectionalParticles={link => highlightLinks.has(link) ? 6 : 0}
        linkDirectionalParticleWidth={4}
        linkDirectionalParticleSpeed={0.006}
        linkDirectionalParticleColor={() => colors.particle}
        onNodeHover={setHoverNode}
        d3VelocityDecay={0.3}
        d3AlphaDecay={0.02}
        cooldownTime={3000}
        warmupTicks={100}
        enableNodeDrag={true}
        enableZoomInteraction={true}
        enablePanInteraction={true}
      />
    </div>
  );
};

export default GraphVisualization;
